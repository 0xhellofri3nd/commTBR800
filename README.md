# commTBR800

Communication with Toshiba TBR800 control


## Instruções para troca de parâmetros via GSM

#### 1º Caractere: _Parâmetro desejado_ (1 À 9)
&nbsp; &nbsp; &nbsp; **[1]** Tensao REF
&nbsp; &nbsp; &nbsp; **[2]** Insensibilidade
&nbsp; &nbsp; &nbsp; **[3]** Teporizacao
&nbsp; &nbsp; &nbsp; **[4]** Limitador Max
&nbsp; &nbsp; &nbsp; **[5]** Limitador Min
&nbsp; &nbsp; &nbsp; **[6]** Bloq. Tap Max
&nbsp; &nbsp; &nbsp; **[7]** Bloq. Tap Min
&nbsp; &nbsp; &nbsp; **[8]** RLV
&nbsp; &nbsp; &nbsp; **[9]** RLI

#### 2º Caractere: _Valor desejado para o parâmetro selecionado_

&nbsp; &nbsp; &nbsp; **Para [1]:** 100 à 130 (passo 1)
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;**Para [2]:** 1.0 à 6.0 (passo 0.1)
&nbsp; &nbsp; &nbsp; **Para [3]:** 10 à 120 (passo 0.1)
&nbsp; &nbsp; &nbsp; **Para [4]:** 120 à 135 (passo 1) ->  10 para desligar
&nbsp; &nbsp; &nbsp; **Para [5]:** 105 à 118  (passo 1) -> 10 para desligar
&nbsp; &nbsp; &nbsp; **Para [6]:** 1 à 5, sendo:
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; **1.** 5%
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; **2.** 6 1/4%
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; **3.** 7 1/2
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; **4.** 8 3/4%
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; **5.**  10% 
&nbsp; &nbsp; &nbsp; **Para [7]:** 1 à 5, sendo:
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; **1.** 5%
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; **2.** 6 1/4%
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; **3.** 7 1/2
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; **4.** 8 3/4%
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; **5.**  10% 
&nbsp; &nbsp; &nbsp; **Para [8]:** 25.0 à 209.0 (passo 0.1)
&nbsp; &nbsp; &nbsp; **Para [9]:** 250 à 6000 (passo 1)
