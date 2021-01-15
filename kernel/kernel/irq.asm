global irq0
global irq1
global irq2
global irq3
global irq4
global irq5
global irq6
global irq7
global irq8
global irq9
global irq10
global irq11
global irq12
global irq13
global irq14
global irq15
 
global load_idt
 
extern irq0_handler
extern irq1_handler
extern irq2_handler
extern irq3_handler
extern irq4_handler
extern irq5_handler
extern irq6_handler
extern irq7_handler
extern irq8_handler
extern irq9_handler
extern irq10_handler
extern irq11_handler
extern irq12_handler
extern irq13_handler
extern irq14_handler
extern irq15_handler

%macro pushaq 0
    push rax
    push rbx
    push rcx
    push rdx
    push rbp
    push rdi
    push rsi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
%endmacro
%macro popaq 0
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rsi
    pop rdi
    pop rbp
    pop rdx
    pop rcx
    pop rbx
    pop rax
%endmacro

irq0:
  pushaq
  call irq0_handler
  popaq
  iret
 
irq1:
  pushaq
  call irq1_handler
  popaq
  iret
 
irq2:
  pushaq
  call irq2_handler
  popaq
  iret
 
irq3:
  pushaq
  call irq3_handler
  popaq
  iret
 
irq4:
  pushaq
  call irq4_handler
  popaq
  iret
 
irq5:
  pushaq
  call irq5_handler
  popaq
  iret
 
irq6:
  pushaq
  call irq6_handler
  popaq
  iret
 
irq7:
  pushaq
  call irq7_handler
  popaq
  iret
 
irq8:
  pushaq
  call irq8_handler
  popaq
  iret
 
irq9:
  pushaq
  call irq9_handler
  popaq
  iret
 
irq10:
  pushaq
  call irq10_handler
  popaq
  iret
 
irq11:
  pushaq
  call irq11_handler
  popaq
  iret
 
irq12:
  pushaq
  call irq12_handler
  popaq
  iret
 
irq13:
  pushaq
  call irq13_handler
  popaq
  iret
 
irq14:
  pushaq
  call irq14_handler
  popaq
  iret
 
irq15:
  pushaq
  call irq15_handler
  popaq
  iret
 
load_idt:
	mov edx, [esp + 4]
	lidt [edx]
  ; TODO: Uncomment this once and for all!
	;sti
	ret