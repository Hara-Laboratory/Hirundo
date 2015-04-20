# HIRUNDO
## A Synthesizable MIPS processor
Described in C and can be Synthesized from HLS tool

                             Memory
  ------------            --------------
  |          |            |  Program   |
  |          |            |------------|
  |          |----------->| SUBLEQ Mem |
  |Processor |            |------------|
  |          |<-----------| SUBLEQ Rou |
  |          |            |------------|
  |          |            |  Reg_File  |
  ------------            --------------
