# Segunda Chamada de Teoria dos Grafos

## Nome

- Pedro Lucas Botelho Freitas - 202135040

## Resumo do Projeto

Este repositório contém o trabalho da disciplina DCC059 - Teoria dos Grafos ministrada pelo Prof. Gabriel Souza no Departamento de Ciência da Computação da UFJF. O objetivo principal do projeto é implementar implementar algoritmos para o problema da Arvore Geradora Minima Generalizada (AGMG)

### Estruturas de Armazenamento
- **grafo_matriz**: Utiliza matriz de adjacência para representar arestas. Em grafos não direcionados, é utilizada a representação linear de uma matriz triangular. Tanto vértices quanto arestas são estáticos.
- **grafo_lista**: Utiliza lista encadeada tanto para vértices quanto para arestas, utilizando alocação dinâmica.

### Funcionalidades Implementadas
As seguintes funções são implementadas na classe abstrata:
- `eh_completo`: Verifica se o grafo é completo.
- `get_grau`: retorna o grau maximo do grafo
- `carrega_grafo`: constrói grafo a partir de um arquivo
- `agmg_gulosa`: implementa um algoritmo guloso para AGMG
- `agmg_randomizada`: implementa um algoritmo randomizado para AGMG
- `agmg_reativo`: implementa um algoritmo reativo para AGMG

### Requisitos e Restrições
- O código deve seguir os princípios de Programação Orientada a Objetos, especialmente herança e encapsulamento.
- Bibliotecas permitidas são `fstream`, `iostream`, `iomanip`, `cmath`, `cstdlib`, `cstdarg`, `ctime`, `string`.
- Não são permitidas arestas múltiplas ou laços nos grafos.
- Documentação completa do código é exigida.

### Execução do Código
A aplicação deve ser compilada e executada via terminal com as seguintes linhas de comando:

1. `main.out -p -m grafo.txt`: Carrega e executa os algoritmos (guloso, reativo ou randomizado) de acordo com sua escolha para o grafo_matriz.
2. `main.out -p -l grafo.txt`: Carrega e executa os algoritmos (guloso, reativo ou randomizado) de acordo com sua escolha para o grafo_lista.

### Envios
O trabalho deve ser enviado em um arquivo zip contendo o código fonte, link para o projeto no GitHub, uma apresentação em PDF, documentação do código, e a identificação dos integrantes com suas contribuições.

### Avaliação
A avaliação do trabalho será realizada com base no relatório (`R`), na avaliação geral do trabalho (`T`), e na participação do aluno no desenvolvimento do código (`P`). A nota final será calculada pela fórmula: `Nota Final = A * T * P`.

## Como rodar o projeto? 

```bash
#adiciona a pasta build para o cmake
 mkdir build
```

```bash
 cd build
```
```bash
#instala as dependencias cmake
 cmake ..
```

```bash
#atualiza dependencia para run
 make
```
```bash
#executa para estrutura de matriz
 time ./main.out -p -m ../entradas/grafo.txt 
```

```bash
#executa para estrutura de lista
 time ./main.out -p -l ../entradas/grafo.txt 
```

##
**Departamento de Ciência da Computação - UFJF**
**DCC059 - Teoria dos Grafos**
**Prof. Gabriel Souza**
