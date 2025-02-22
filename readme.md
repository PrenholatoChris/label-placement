# Algoritmo Genético para Otimização de Conflitos

Este projeto implementa um algoritmo genético para resolver o problema de posicionamento de rótulos cartográficos de pontos (Point-Feature Cartographic Label Placement - PFCLP), maximizando o número de rótulos posicionados sem sobreposição. O modelo considera a formulação de um problema de otimização 0-1 de programação linear inteira e implementa técnicas de decomposição para melhorar a solução.

## Funcionalidades
- Geração de uma população inicial aleatória.
- Avaliação de soluções com base na função objetivo (FO).
- Cruzamento entre soluções para gerar novas soluções.
- Mutação para diversificação da população.
- Inserção ordenada da população para manter apenas as melhores soluções.
- Impressão da melhor solução encontrada ao atingir o tempo limite.

## Compilação e Execução

### Compilação
Para compilar o programa, utilize um compilador **g++**:

```sh
 g++ main.cpp -o  main.exe
```

### Execução
Após a compilação, execute o programa passando os parâmetros necessários:

```sh
./main.exe <fileName> <total_generations> <initial_pop> <max_pop> <total_crossings> <mutation_rate> <time_limit>
```

### Exemplo de Uso:
```sh
./main.exe pfclp/d25_01.dat 5000 50 200 20 0.5 300
```

## Parâmetros
1. **fileName**: Caminho para o arquivo de entrada contendo os dados do problema.
2. **total_generations**: Número total de gerações que o algoritmo deve executar.
3. **initial_pop**: Número inicial de soluções geradas aleatoriamente.
4. **max_pop**: Número máximo de soluções na população.
5. **total_crossings**: Número de cruzamentos realizados a cada geração.
6. **mutation_rate**: Probabilidade de mutação aplicada a cada solução.
7. **time_limit**: Tempo máximo de execução da heurística.


## Contribuição
Sinta-se à vontade para contribuir com melhorias ou otimizações para o projeto. Basta abrir um pull request com as sugestões.

## Licença
Este projeto é de código aberto e pode ser utilizado livremente para fins educacionais e acadêmicos.

---

Caso tenha dúvidas ou sugestões, fique à vontade para entrar em contato! 🚀

