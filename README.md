Este programa permite editar a banlist de arquivos .ygl criados para os jogos Yu-Gi-Oh! Tag Force de PSP como DLC. A ferramenta foi desenvolvida com base nas descobertas feitas por Chrisfand, e eu apenas automatizei o método de edição que ele criou, com o objetivo de facilitar o processo.

O programa é capaz de editar a banlist, mas é necessário usar um cheat para que o jogo reconheça a DLC modificada. Atualmente, há um cheat disponível apenas para Tag Force 1, que também foi desenvolvido por Chrisfand, então todos os créditos a ele. Abaixo segue o cheat necessário para que a banlist alterada funcione na versão europeia do Tag Force 1:

Cheat para Habilitar Suporte a DLC Personalizado (Receitas/Banlists/Cards) - Funciona em TF1 EUR com JP:

_C1 Allows Game To Read Decrypted DLC Files
_L 0xD006B9EC 0x2000AE98
_L 0x2006B980 0x34020001
_L 0xD006B906 0x20002902
_L 0xD006B904 0x20000004
_L 0x2006B904 0x29020003

Estou tentando adaptar esse cheat para o Yu-Gi-Oh! Arc-V Tag Force Special, então qualquer ajuda é muito bem-vinda.

Abaixo estão as instruções para usar o programa.

Use o Programa no CMD

como o CMD ou Terminal:

Windows: Pressione Win + R, digitecmd e pressione Enter.
Linux/macOS: Abra o terminal usando o menu de aplicativos ou Ctrl + Alt + T.
Navegando até o diretório do executável:

Use cd para acessar o diretório do programa

cd C:\caminho\do\programa

Comandos para Executar o Programa:

Para extrair a banlist do arquivo .ygl:

banlist_editor.exe extract SampleBanList.YGL banlist.txt

Para comprimir uma banlist a partir deo .txt:

banlist_editor.exe compress banlist_input.txt SampleBanList.YGL

Como Editar o Arquivo TXT
Estrutura de Exemplo do arquivo do txt:

CardCount: 3

Forbidden:
CardID: 4007 (Blue-Eyes White Dragon)

Limited:
CardID: 4008 (Mystical Elf)

Semi-Limited:
CardID: 4009 (Hitotsu-Me Giant)

Instruções de Edição
Adicionar uma Carta:

Escolha (Forbidden, Limited ou Semi-Limited).
Adicione uma nova linha com o formato:

CardID: 4010 (Dark Magician)
