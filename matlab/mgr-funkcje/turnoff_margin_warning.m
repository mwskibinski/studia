function [] = turnoff_margin_warning()
    warning('off', 'Control:analysis:MarginUnstable');