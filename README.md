# TCP Project - MC833

Compare o uso de sockets para comunicação cliente com
- servidor concorrente sobre TCP, e 
- servidor iterativo sobre UDP

Parte 1 (TCP): 
O servidor armazena informações sobre disciplinas: título, ementa, sala de aula, horário, código da disciplina (identificador). Adicionalmente o servidor armazena um texto de comentário sobre a próxima aula de uma disciplina escrito pelo cliente professor. Portanto, existem dois tipos de usuário: aluno e professor. Pelo menos, as seguintes operações devem ser realizadas por um usuário:
  - listar todos os códigos de disciplinas com seus respectivos títulos;
  - dado o código de uma disciplina, retornar a ementa;
  - dado o código de uma disciplina, retornar todas as informações desta disciplina;
  - listar todas as informações de todas as disciplinas;
  - escrever um texto de comentário sobre a próxima aula de uma disciplina (apenas usuário professor);
  - dado o código de uma disciplina, retornar o texto de comentário sobre a próxima aula.
