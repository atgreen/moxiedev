;;; 1010101010101010101010101010101010101010101010101010101010101010101010101010101
;;; 0101010101010101010101010101010101010101010101010101010101010101010101010101010
;;; 1   ___  ___          _     ______                                            1
;;; 0   |  \/  |         (_)    |  _  \              Open Source Tools,           0
;;; 1   | .  . | _____  ___  ___| | | |_____   __    Firmware, and HDL code for   1
;;; 0   | |\/| |/ _ \ \/ / |/ _ \ | | / _ \ \ / /    the Moxie processor core.    0
;;; 1   | |  | | (_) >  <| |  __/ |/ /  __/\ V /                                  1
;;; 0   \_|  |_/\___/_/\_\_|\___|___/ \___| \_/      http://moxielogic.org/blog   0
;;; 1                                                                             1
;;; 0101010101010101010101010101010101010101010101010101010101010101010101010101010
;;; 1010101010101010101010101010101010101010101010101010101010101010101010101010101

;;; Copyright (C) 2012  Anthony Green <green@moxielogic.com>
;;; Distrubuted under the terms of the GPL v3 or later.

;;; This program reads an input file called microcode.org, which
;;; contains an emacs org-mode table with moxie microcode information.
;;; It parses this table and produces a microcode ROM file
;;; microcode.bin, suitable for inclusion in the moxie core RTL.
;;;

;;; Thanks for Zach Beane for quicklisp (which you'll need to install)
;;;

(ql:quickload "cl-ppcre")
(ql:quickload "trivial-bit-streams")

(defpackage :microcoder
  (:use :cl :cl-user :cl-ppcre :trivial-bit-streams))

(in-package :microcoder)

(let ((in (open "microcode.org" :if-does-not-exist nil))
      (opcode-array (make-array 64 :initial-element nil)))
  (when in
    ;; skip to table contents
    (loop for filepos = (file-position in)
	  for line = (read-line in nil)
 	  until (let ((s (cl-ppcre:split "\\|" line)))
		  (equal (length s) 7)))
    (read-line in nil)

    ;; We're at the table contents now.  Parse it and write our new
    ;; microcode.bin file.
    (with-open-file 
     (out "microcode.bin" :direction :output
	  :element-type '(unsigned-byte 8)
	  :if-exists :supersede)
     (with-bit-output-stream 
      (bs :callback (make-stream-output-callback out))
      (loop for filepos = (file-position in)
	    for line = (read-line in nil)
	    while line do 
	    (let ((s (cl-ppcre:split "\\|" line)))
	      (if (equal 7 (length s))
		  (destructuring-bind (junk1 name code wreg? rA? rb? &rest junk2) 
				      (mapcar (lambda (v) (string-trim " " v)) s)
				      (setf (aref opcode-array (parse-integer code :radix 2))
					    (list name wreg? rA? rb?))))))
      (loop for i from 0 to 63
	    do (let ((o (aref opcode-array i)))
		 (if o
		     (mapcar (lambda (v)
			       (let ((n (parse-integer v :radix 2)))
				 (cond 
				  ((equal n 0)
				   (write-sequence #*0 bs))
				  ((equal n 1)
				   (write-sequence #*1 bs))
				  (t
				   (error "bad table entry")))))
			     (cdr o))
		   (write-sequence #*000 bs))))))
      
    (close in)))
    
(sb-ext:quit)

