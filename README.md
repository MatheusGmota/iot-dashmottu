# Projeto DashMottu: Simulação de Localização de Motos com ESP32 e Firebase

Este projeto simula um sistema de localização de motos usando um ESP32 como gateway e o Firebase Realtime Database para exibir a posição em tempo real. A simulação é feita de forma simples, onde botões representam "âncoras" que, ao serem pressionadas, enviam dados de localização pré-definidos para o gateway.

## 👩‍👦‍👦 Equipe
* Felipe Seiki Hashiguti - RM98985
* Lucas Corradini Silveira - RM555118
* Matheus Gregorio Mota - RM557254

## Tecnologias Utilizadas

* **ESP32:** Um microcontrolador versátil com Wi-Fi e Bluetooth, atuando como o **Gateway Principal** que recebe os dados (simulados) das âncoras e os envia para a nuvem.
* **Firebase Realtime Database:** Um banco de dados NoSQL baseado em nuvem fornecido pelo Google, ideal para armazenar e sincronizar dados em tempo real. Utilizamos para **visualizar a posição da moto** em um dashboard web.
* **PlatformIO (VS Code Extension):** Um ambiente de desenvolvimento integrado para embarcados que facilita a compilação e o gerenciamento de bibliotecas para o ESP32.
* **Wokwi (VS Code Extension):** Um simulador de hardware online que permite rodar o código do ESP32 e ver o circuito em ação sem a necessidade de um dispositivo físico.

## Como a Simulação Acontece

Neste projeto, simplificamos a lógica de localização para focar no fluxo de dados. Ao invés de usar módulos Ultra-Wideband (UWB) para medição de distância e algoritmos de trilateração complexos, a simulação funciona assim:

1.  **Âncoras Virtuais (Pushbuttons):** Três pushbuttons (botões) são conectados ao ESP32 no simulador. Cada botão representa uma "âncora" UWB no ambiente físico.
2.  **Envio de Posição Mockada:**
    * Quando você pressiona o **Botão 1**, o ESP32 "entende" que a moto está em uma **Posição X e Y pré-definida (ex: 5.0m, 5.0m)** e envia esses dados para o Firebase.
    * Ao pressionar o **Botão 2**, uma **Posição X e Y diferente (ex: 15.0m, 5.0m)** é enviada.
    * E o **Botão 3** envia uma terceira **Posição X e Y (ex: 10.0m, 12.0m)**.
3.  **Gateway Principal (ESP32):** O ESP32, configurado como o gateway, é responsável por:
    * Monitorar o estado dos botões.
    * Ao detectar um botão pressionado, ele associa essa ação a uma posição específica.
    * Envia essa posição (X e Y) para o Firebase Realtime Database.
4.  **Visualização no Firebase:** No console do Firebase, você pode acompanhar em tempo real as coordenadas X e Y da moto sendo atualizadas a cada clique nos botões, simulando o movimento da moto pelo "pátio".

Essa abordagem permite demonstrar o fluxo completo de dados do dispositivo embarcado para a nuvem de forma prática e fácil de entender.

## Como Rodar o Projeto

### Pré-requisitos

* **VS Code:** Instale o Visual Studio Code.
* **PlatformIO IDE Extension:** Instale a extensão "PlatformIO IDE" no VS Code.
* **Wokwi for PlatformIO Extension:** Instale a extensão "Wokwi for PlatformIO" no VS Code.
* **Conta Firebase:** Crie um projeto no Firebase e configure um **Realtime Database** (inicie no modo de teste para facilitar a simulação). Obtenha o `FIREBASE_HOST` (o ID do seu projeto no formato URL) e o `FIREBASE_AUTH` (sua chave secreta do banco de dados).

### Configuração do Projeto

1.  **Clone ou Baixe o Projeto:**
    ```bash
    git clone https://github.com/MatheusGmota/IOT-dashmottu.git
    cd esp32-Dashmottu
    ```
    (Se não estiver usando Git, apenas descompacte o arquivo zip.)

2.  **Abrir no VS Code:**
    * No VS Code, vá em `File > Open Folder...` e selecione a pasta raiz do projeto (`esp32-Dashmottu`).

3.  **Configurar Credenciais do Firebase (`src/main.cpp`):**
    * Abra o arquivo `src/main.cpp`.
    * **SUBSTITUA** `YOUR-PROJECT-ID-default-rtdb.firebaseio.com` pelo `FIREBASE_HOST` real do seu projeto Firebase.
    * **SUBSTITUA** `YOUR_DATABASE_SECRET_KEY` pela sua `FIREBASE_AUTH` (chave secreta do banco de dados) real.

    ```cpp
    #define FIREBASE_HOST "SEU_FIREBASE_PROJECT_ID-default-rtdb.firebaseio.com"
    #define FIREBASE_AUTH "SUA_CHAVE_SECRETA_DO_FIREBASE"
    ```

### Rodando a Simulação

1.  **Compilar o Projeto:**
    * No VS Code, na barra inferior do PlatformIO, clique no ícone de **"Build" (o visto)** ou use `Ctrl+Alt+B`.

2.  **Iniciar o Simulador Wokwi:**
    * Clique no ícone verde de play
    * Pressione `F1` (ou `Ctrl+Shift+P`) para abrir a Paleta de Comandos.
    * Digite `Wokwi: Start Simulator` e selecione-o.

4.  **Interagir com a Simulação:**
    * A janela do simulador Wokwi aparecerá no VS Code. Nela, você verá o ESP32 e os botões.
    * No painel inferior da janela do simulador Wokwi, clique na aba **"Serial Monitor"**. Você verá as mensagens de inicialização do ESP32.
    * **Pressione os botões** no diagrama do simulador. Você verá mensagens no Serial Monitor e, ao mesmo tempo, os dados de `posicaoX` e `posicaoY` serão atualizados no seu **Firebase Realtime Database** (acesse-o via web no console do Firebase).
