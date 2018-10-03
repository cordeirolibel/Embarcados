; Autores: Igor Pardal Latini, Gustavo Cordeiro Libel, Mateus Campaner Hercules

                PUBLIC median

        SECTION .text : CODE (2)
        THUMB

/*Registradores:
  R0 = Dimensão
  R1 = Vetor de entrada
  R2 = Vetor de saída (...  e 2, para a divisão)
  R3 = h
  R4 = i
  R5 = j
  R6 = Valores do vetor na posicao i
  R7 = Valores do vetor na posicao j-h
  R8 = j-h
*/

median:  
        // Não estraga os registradores 
        PUSH {R4 - R8, LR}
        // preciso da dimensão no final para copiar o vetor organizado para o vetor de saída
        PUSH {R0, R2}
      
        // h = int(dim/2)
        MOV R2, #2
        ;UDIV R3, R0, R2
        // ajuste heurístico
        MOV R3, #250
        
        // necessidade heurística
        MOV R2, #22
        MOV R10, #10

comparacaoh:
        // h>0
        CMP R3, #0
        BLS fim
        
        // i = h
        MOV R4, R3
        B comparacaoi
        
 cortah:
        ; // h = int(h/2)
        ;UDIV R3, R3, R2

        // ajuste heurístico
        MUL R3, R3, R10
        UDIV R3, R3, R2
        
        B comparacaoh

comparacaoi:
        // i<dim
        CMP R4, R0
        BHS cortah 
        
        // c = vec_in[i]
        LDRB R6, [R1, R4]
        
        // j = i
        MOV R5, R4

compdupla:
        // j>=h and c<vec_in[j-h]
        SUB R8, R5, R3
        LDRB R7, [R1, R8]
        CMP R5, R3
osdois:
        BHS talvezfazmudanca
mudai:       
        // i+= 1
        ADD R4, R4, #1
        B comparacaoi
        
       
talvezfazmudanca:
        // and c<vec_in[j-h]
        CMP R6, R7
        BHS mudai
        
fazmudanca:
        // vec_in[j] = vec_in[j-h]
        STRB R7, [R1, R5]
        
        // j = j-h
        SUB R5, R5, R3
        
        // vec_in[j] = c
        STRB R6, [R1, R5]
        B compdupla
        
fim:
        
        LSR R3, R0, #1
        
        // dim%2 == 0
        AND R0, R0, #1
        CMP R0, #1
        BEQ impar
        
        // mediana = (vec_out[pos-1] + vec_out[pos])/2
        SUB R4, R3, #1 // R4 agora é pos-1
        LDRB R5, [R1, R4]
        LDRB R6, [R1, R3]
        ADD R0, R5, R6
        LSR R0, R0, #1
        B tafeito
        
impar:
        LDRB R0, [R1, R3]
        
tafeito:
        MOV R6, R0
	POP {R0, R2}
        MOV R4, #0
        
        
loopCopia:
        CBZ R0, retorno
        // copio do vetor de entrada para o de saida
        LDRB R5, [R1, R4]
        STRB R5, [R2, R4]
        
        ADD R4, R4, #1
        SUB R0, R0, #1
        B loopCopia

retorno:
        MOV R0, R6
        // Volta aos valores anteriores
        POP {R4 - R8, LR}
        BX LR
        
        END
