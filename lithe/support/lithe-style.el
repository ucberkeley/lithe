;;; lithe-style.el --- support for Lithe style in CC Mode

;; To load this file at startup time add the following to your .emacs:
;;   (load "/path/to/lithe-style.el")
;;
;; In addition, you can add lithe-c-mode-common-hook by adding:
;;   (add-hook 'c-mode-common-hook 'lithe-c-mode-common-hook)

(defconst lithe-c-style
  '((c-basic-offset . 2)
    (c-hanging-braces-alist . ((block-close)
			       (brace-list-open)
			       (brace-list-close)
			       (substatement-open after)
			       (statement-case-open after)
			       (class-open after)
			       (class-close before)
			       (extern-lang-open after)
			       (extern-lang-close before after)
			       (namespace-open after)
			       (namespace-close before after)
			       ))
    (c-offsets-alist . ((case-label . *)
			(statement-case-intro . *)
			(statement-case-open . *)
			(access-label . /)
			(inextern-lang . 0)
			(innamespace . 0)
			))
    (c-cleanup-list . (brace-else-brace
		       ))
    )
  "Lithe C Style"
  )

(c-add-style "lithe" lithe-c-style)

(defun lithe-c-mode-common-hook ()
  ;; Use the Lithe C style.
  (c-set-style "lithe")

  ;; Turn on auto-newlines.
  (c-toggle-auto-state 1)

  ;; Get rid of auto-newlines after each semicolon!
  (set 'c-hanging-semi&comma-criteria nil)

  ;; Require a new line at the end of the file.
  (set 'require-final-newline t)

  ;; Turn on auto-fill mode to help comments wrap.
  (auto-fill-mode)
)
