(*+  TEST4 - cons sequence nested in loop TRUE +*)
PROGRAM TEST4;
CONST
A0 = 3;
GPR = 45;
BEGIN
WHILE  A0 <= 44
DO
IF 44 <> 44
THEN
	WHILE GPR = A0
	DO
	ENDWHILE;
ENDIF;
IF 44 <> 44
THEN
	WHILE GPR = A0
	DO
	ENDWHILE;
ENDIF;
ENDWHILE;
END.
