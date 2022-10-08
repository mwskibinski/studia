/*
 * Ring buffer character device - example for Operating System course
 *
 * Copyright (C) 2016, 2017, 2018  Krzysztof Mazur <krzysiek@podlesie.net>
 *
 * Add your copyright here and change MODULE_AUTHOR.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#define pr_fmt(fmt) "ringdev: " fmt
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/sched.h>
#include <linux/uaccess.h>

#define RINGDEV_BUF_SIZE 4096

// Koleka dla odczytu.
static DECLARE_WAIT_QUEUE_HEAD(queue_read);

static struct mutex ringdev_lock;

/* Bufor cykliczny z wykorzystaniem dwoch wskaznikow i zadnych zmiennych poza tym.
 * Trace jeden bajt na rzecz rozrozniania pomiedzy stanami pelny/pusty.
 * Wskaznik zapisu jest jeden.
 * Wskaznik odczytu to w tym wypadku offset wzgledem poczatku bufora.
 *
 * Offset wzgledem bufora jest obliczany na etapie zapisu. Offset ten wlasciwie wskazuje na wsk. zapisu. 
 * W przypadku zadan z jednym procesem czytajacym i zapisujacym. Mozna bylo wykorzystac dodatkowy wsk. do odczytu tak by poprawnie uwzglednic to co UDALO sie zapisac.
 * W przypadku wielu procesow czytajacych, trudno jest ustalic jeden wskaznik odpowiadajacy za to co udalo sie odczytac.
 * Zatem, jesli ktores z zadan nie odczyta jakis bajtow to straci je, poniewaz 'wsk. odczytu' bedzie wskazywal zawsze tak jakby wszystko przebieglo pomyslnie.
 */
static char ringdev_buf[RINGDEV_BUF_SIZE];
static char *wr_ptr = ringdev_buf;

static int ringdev_open(struct inode *inode, struct file *filp) {
	filp->private_data = wr_ptr; // Kazdy proces dostaje aktualny wskaznik odczytu i wzgledem niego czyta dalej.
	return 0;
}

static ssize_t ringdev_read(struct file *filp, char __user *buf, size_t count, loff_t *off) {
	ssize_t ret = 0;
	size_t n = 0;
	bool wait_interrupted = false; // Pozwala rozroznic czy odczyt zostal zatrzymany sygnalem.

	/* Wskaznik odczytu zabieram ze struktury file. Tworze dodatkowa zmienna by jak najmniej meczyc sie przy konwertowaniu void *, czy dobieraniu sie do pol struktury.
	 * Tworze ja takze by jak najmniej zmieniac moj poprzedni kod, ktory obslugiwal bufor cykliczny.
	 */
	char *rd_ptr = (char *) filp->private_data;

read:

	/* Zaczynam od przejecia mutexa i sprawdzeniu ile mozna odczytac. Rozrozniam to jak wskazniki sa polozone wzgledem siebie.
	 * Jesli nic to wchodze do instrukcji if i czekam az sytuacja ulegnie zmianie.
	 * Jesli ulegnie to wracam na poczatek by sprawdzic czy na pewno caly czas jest cos do odczytu.
	 */
	mutex_lock(&ringdev_lock);
	n = (wr_ptr >= rd_ptr ? wr_ptr - rd_ptr : RINGDEV_BUF_SIZE - (rd_ptr - wr_ptr));

	if(!n) {
		mutex_unlock(&ringdev_lock);

		/* Staram sie byc uwazny przy usypianu i uzywaniu mutexa.
		 * Jesli wykonuje sie usypianie to zwalniam mutexa.
		 * W przypadku przerwania snu sygnalem uzywam pomocniczej zmiennej logicznej, ktora pozwala mi na nie wykonywanie zwalniania mutexu na koncu kodu.
		 * Jesli dobrze przeczytalem mutexu nie wolno zwalniac gdy go sie nie uzywa to tez uzywam takiego sposobu na zachowanie bezpieczenstwa.
		 */
		if(wait_event_interruptible(queue_read, wr_ptr != rd_ptr) != 0) {
			wait_interrupted = true;
			ret = -ERESTARTSYS;
			goto end;
		}
		else
			goto read;
	}

	n = n > count ? count : n; // Porownuje ilosc bajtow mozliwych do zapisu z iloscia miejsca po stronie uzytkownika.

	/* Sytuacja 'latwiejsza': Wskaznik odczytu jest przed wsk. zapisu.
	 * Czytaj az do niego dojdziesz.
	 * W przypadku nie odczytania niczego zwroc blad.
	 */
	if(wr_ptr >= rd_ptr) {
		ret = n - copy_to_user(buf, rd_ptr, n);
		if(!ret) {
			ret = -EFAULT;
			goto end;
		}

		rd_ptr += ret; // Zmien ustawienie wskaznika do miejsca do ktorego udal sie odczyt.
	}

	/* Sytuacja 'trudniejsza': Odczyt wykonany w dwoch czesciach, najpierw od wskaznika read do konca bufora, a potem od poczatku do wskaznika write.
	 * Zmienna temp najpierw oblicza ilosc znakow do konca bufora.
	 * Jesli nic nie udalo sie odczytac to koncz z bledem, jesli sie udalo, ale nie wszystko to zakoncz juz i nie wykonuj drugiej czesci.
	 */
	else {
		size_t temp = ringdev_buf + RINGDEV_BUF_SIZE - rd_ptr;
		ret = temp - copy_to_user(buf, rd_ptr, temp);
		if(!ret) {
			ret = -EFAULT;
			goto end;
		}
		else if(ret != temp) {
			rd_ptr += ret;
			goto end;
		}

		/* Zmniejszamy ilosc znakow do odczytu o wartosc odczytana.
		 * Pozniej temp przechowuje informacje ile udalo sie odczytac w tej drugiej czesci od poczatku bufora.
		 */
		n -= ret;
		temp = n - copy_to_user(buf + ret, ringdev_buf, n);
		ret += temp;

		rd_ptr = ringdev_buf + temp; // Przesun wskaznik wzgledem poczatku bufora o znaki przeczytane w drugiej 'turze'.
	}

end:
	if(!wait_interrupted) // Wykrywam czy powinienem zwolnic mutex.
		mutex_unlock(&ringdev_lock);
	filp->private_data = rd_ptr; // Zaktualizuj wskaznik w strukturze file.
	return ret;
}

static ssize_t ringdev_write(struct file *filp, const char __user *buf,	size_t count, loff_t *off) {
	ssize_t ret = 0;
	char *rd_ptr;
	size_t n;

	// Zaczynam od przejecia mutexu.
	mutex_lock(&ringdev_lock);

	/* Analogicznie do odczytu najpierw obliczamy ilosc dostepnego miejsca w buforze.
	 * Tu takze rozniamy dwie sytuacje miejsca wskaznikow.
	 * Jako ze do rozroniania sytuacji pelny/pusty to zawsze odejmuje ten jeden bajt.
	 * Pozniej porownuje z iloscia ktora faktycznie uzytkownik chce zapisac.
	 */
	rd_ptr = ringdev_buf + (ringdev_buf - wr_ptr);

	n = (rd_ptr > wr_ptr ? rd_ptr - wr_ptr - 1 : RINGDEV_BUF_SIZE - (wr_ptr - rd_ptr) - 1);
	n = n > count ? count : n;

	// Jesli nie ma nic do zapisu to koncz. Count, zamiast n, uzyty celowo co widac w nastepnym warunku.
	if(!count)
		goto end;

	// Jesli nie ma miejsca w buforze
	if(!n) {
		ret = -ENOSPC;
		goto end;
	}

	/* Analogicznie do odczytu sa dwie sytuacje, tym razem troszke inne. Tym razem nie sprawdzamy polozenia wskaznikow, 
	 * a czy jest miejsce zapisac wszystko od wsk. zapisu do konca bufora.
	 * Jesli tak to zapis jest wykonany w jednej turze. Sprawdzamy czy udalo sie cos zapisac, a jesli nic to zwracamy blad.
	 * W przypadku zapisu 'na styk' z koncem bufora, wlacza sie ten drugi przypadek, by poprawnie umiescic wskaznik na poczatek bufora.
	 */
	if((wr_ptr + n) < (ringdev_buf + RINGDEV_BUF_SIZE)) {
		ret = n - copy_from_user(wr_ptr, buf, n);
		if(!ret) {
			ret = -EFAULT;
			goto end;
		}

		wr_ptr += ret;
	}	
	else {
		/* Temp przechowuje ilosc bajtow do konca bufora.
		 * Jesli zapis sie w ogole nie udal --> blad -EFAULT, a jesli nie calkowicie --> konczymy wczesniej pamietajac o zmianie polozenie wskaznika.
		 */
		size_t temp = ringdev_buf + RINGDEV_BUF_SIZE - wr_ptr;
		ret = temp - copy_from_user(wr_ptr, buf, temp);
		if(!ret) {
			ret = -EFAULT;
			goto end;
		}
		else if(ret != temp) {
			wr_ptr += ret; 
			goto end;
		}

		/* Druga partia - od poczatku bufora.
		 * Analogicznie do odczytu.
		 */
		n -= ret;
		temp = n - copy_from_user(ringdev_buf, buf + ret, n);
		ret += temp;

		wr_ptr = ringdev_buf + temp;
	}

end:
	mutex_unlock(&ringdev_lock);
	if(ret > 0) // Jesli jakies dane zostaly zapisane to obudz procesy czytajace.
		wake_up_interruptible(&queue_read);
	return ret;
}

static int ringdev_release(struct inode *inode, struct file *filp) {
	return 0;
}

static const struct file_operations ringdev_fops = {
	.owner = THIS_MODULE,
	.open = ringdev_open,
	.read = ringdev_read,
	.write = ringdev_write,
	.release = ringdev_release,
};

static struct miscdevice ringdev_miscdevice = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "ringdev",
	.fops = &ringdev_fops
};

static int __init ringdev_init(void) {
	int ret;

	mutex_init(&ringdev_lock);
	ret = misc_register(&ringdev_miscdevice);
	if (ret < 0) {
		pr_err("can't register miscdevice.\n");
		return ret;
	}

	pr_info("minor %d\n", ringdev_miscdevice.minor);

	return 0;
}

static void __exit ringdev_exit(void) {
	misc_deregister(&ringdev_miscdevice);
}

module_init(ringdev_init);
module_exit(ringdev_exit);

MODULE_DESCRIPTION("SO Lab. - Kod koncowy v2");
MODULE_AUTHOR("Maksymilian Skibinski, Jakub Debosz");
MODULE_LICENSE("GPL");
