# Computa el fibonacci recursivo
def fib(x)
	if x < 3 then
		1
	else
		fib(x-1) + fib(x-2)
#Devuelve el fibonacci del 40vo número
fib(40)