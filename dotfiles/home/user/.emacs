(custom-set-variables
 ;; custom-set-variables was added by Custom.
 ;; If you edit it by hand, you could mess it up, so be careful.
 ;; Your init file should contain only one such instance.
 ;; If there is more than one, they won't work right.
 '(custom-safe-themes
   '("8ca8fbaeaeff06ac803d7c42de1430b9765d22a439efc45b5ac572c2d9d09b16" default))
 '(global-display-line-numbers-mode t)
 '(inhibit-startup-screen t)
 '(package-selected-packages
   '(gnu-apl-mode slime powerline org neotree mood-one-theme markdown-mode humanoid-themes gnu-elpa geiser evil color-theme-approximate)))
(custom-set-faces
 ;; custom-set-faces was added by Custom.
 ;; If you edit it by hand, you could mess it up, so be careful.
 ;; Your init file should contain only one such instance.
 ;; If there is more than one, they won't work right.
 )

(tool-bar-mode -1)

(setq inferior-lisp-program "sbcl")

(require 'package)
(add-to-list 'package-archives '("melpa" . "https://melpa.org/packages/") t)
(package-initialize)

(load-theme (quote humanoid-dark))

(global-set-key "\C-x\C-m" 'compile)

;;(evil-mode)

;;(neotree)
(global-set-key "\C-x\C-n" 'neotree-toggle)

(org-babel-do-load-languages
  'org-babel-load-languages
  '((awk . t)
    (C . t)
    (fortran . t)
    (gnuplot . t)
    (java . t)
    (lisp . t)
    (maxima . t)
    (python . t)
    (scheme . t)
    (emacs-lisp . t)
  )
)

(autoload 'maxima-mode "maxima" "Maxima mode" t)
(autoload 'maxima "maxima" "Maxima interaction" t)
(autoload 'maxima-mode "maxima" "Maxima mode" t)
(autoload 'imaxima "imaxima" "Frontend for maxima with Image support" t)
(autoload 'maxima "maxima" "Maxima interaction" t)
(autoload 'imath-mode "imath" "Imath mode for math formula input" t)
(setq imaxima-use-maxima-mode-flag t)
(add-to-list 'auto-mode-alist '("\\.ma[cx]\\'" . maxima-mode))

(defun gober () (interactive) (insert-char 63))
(global-set-key "\C-c\C-c" 'gober)

