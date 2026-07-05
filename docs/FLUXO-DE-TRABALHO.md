# 🔄 Fluxo de trabalho: Issues + Agentes

O ciclo é sempre o mesmo, repetido a cada tarefa:

```
1. ISSUE    →  Descreve UMA tarefa pequena, com Definition of Done clara.
2. BRANCH   →  Um agente cria uma branch a partir da main:  feat/<n>-descricao
3. CÓDIGO   →  O agente implementa APENAS o escopo da issue.
4. PR       →  Abre um Pull Request com "Closes #N" no corpo.
5. REVIEW   →  Você revisa o diff, compila e testa rodando o jogo.
6. MERGE    →  Aprovado, faz merge na main. A issue fecha sozinha.
7. REPETE   →  Próxima issue.
```

## Anatomia de uma boa Issue

- **Título curto e imperativo**: "Adicionar movimento do player com WASD".
- **Contexto**: por que essa tarefa existe / de qual marco faz parte.
- **Definition of Done**: lista de checkboxes verificáveis.
- **Como testar**: passos concretos pra confirmar que funciona.
- **Labels**: `marco:M2`, `tipo:feature`, etc.

## Convenções deste repositório

| Item            | Convenção                                  | Exemplo                         |
|-----------------|--------------------------------------------|---------------------------------|
| Branch          | `feat/<issue>-<slug>`                       | `feat/2-player-movement`        |
| Commit          | `tipo: descrição curta (#issue)`            | `feat: movimento WASD (#2)`     |
| PR fecha issue  | `Closes #N` no corpo do PR                  | `Closes #2`                     |

## Regra de ouro

> **Escopo pequeno.** Uma issue que mexe em um só arquivo e faz uma só coisa é fácil
> de revisar, difícil de quebrar, e permite rodar vários agentes em paralelo sem conflito.
