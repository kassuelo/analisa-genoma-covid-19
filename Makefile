
all: compile

compile:
	clear
	gcc -o executar_analise analisar_genoma.c -lpthread -lm

clean:
	rm -rf ?