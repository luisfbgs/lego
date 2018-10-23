# Lego

Montador e ligador de uma linguagem assembler simplificada. Desenvolvido para a disciplina de Software Básico, da Universidade de Brasília.

---

## Membros
 * Luis Felipe 16/0071569
 * Mikael Mello 16/0015537

## Como usar
Para executar o montador, mude o diretório para `assembler/` e execute `make` para compilar

```bash
$ cd assembler
$ make
$ ./montador <nome do arquivo>
```

Para ligar arquivos objetos, mude o diretório para `linker/` e execute `make` para compilar

```bash
$ cd linker
$ make
$ ./ligador <arquivo1> [<arquivo2>] [<arquivo3>] [<arquivo4>]
```