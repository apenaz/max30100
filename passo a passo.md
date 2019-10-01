## Ligação com o arduino
![fonte: http://mh.nodebb.com](./img/ligacao.jpg)

A ligação de alimentação deve ser realizada no 3.3V e deve-se utilizar os 2 diodos para que a tensão de alimentação fique dentro do limite do sensor MAX.

a ligação deve ser realizada sa seguinte foram para o caso do ESP

* SDA --> D2 (GPIO4)

* SCL --> D1 (GPIO5)

* VIN --> 3.3V

* GND --> GND

## Programação

Após baixar o código do repositorio do Git, caso queira, pode-se alterar o email e a senha para as credenciais do usuário.

É importante alterar o Serial para que o dispositivo tenha um número de série único


O procedimento de cadastrar no wifi pode ser visto  no [site da biblioteca wifimanager](https://github.com/tzapu/WiFiManager)



Com o objetivo de facilitar o uso do sistema  e o desenvolvimento de novos dispositivos foi desenvolvido, pela equipe de sistemas embarcados do projeto, uma biblioteca que pode ser baixada na página do [github](https://github.com/daniellycosta/SaiotDevice)


Para detalhes do sistema 
Para utilizar o sensor é necessário baixar da biblioteca do github e extrair na pasta de bibliotecas da IDE ou do projeto.
A biblioteca pode ser encontrada no [github da fabricante](https://github.com/MHEtLive/MH-ET-LIVE-max30100)

Neste projeto já estão inclusos a biblioteca e o código fonte foi retirado do exemplo "MAX30100_Minimal".


## Referência

http://mh.nodebb.com/topic/43/new-heart-rate-sensor-mh-et-live-max30100

https://github.com/MHEtLive/MH-ET-LIVE-max30100

https://saiot.ect.ufrn.br