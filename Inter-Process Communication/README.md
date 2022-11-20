# Free_RTOS
Practice For Free RTOS

## Task1 : 
Use an existing RTOS project to write a program with two tasks. A task to read a button state and a task to turn an LED ON/OFF. The LED shall toggle its state between ON/OFF with every press on the button. The action of toggling the LED shall take place after the user presses the button once and releases it

## Task2 : 
Use an existing RTOS project to write a program with 2 tasks. Both tasks are responsible for writing on UART any string 10 times each task cycle. The task cycles are, respectively, 100ms, and 500ms. The task with cycle 500ms shall have an empty loop that loops 100,000 times (To simulate heavy load) after every time a string is written on UART. Each task writing to UART shall not be interrupted by the other taskUse an existing RTOS project to write a program with two tasks. A task to read a button state and a task to turn an LED ON/OFF. The LED shall toggle its state between ON/OFF with every press on the button. The action of toggling the LED shall take place after the user presses the button once and releases it

## Task3 :
Use an existing RTOS project to write a program with 4 tasks. Two tasks will detect rising and falling edges on two buttons. Every edge is an event that will be sent to a consumer task to write on UART. A task will send periodic string every 100ms to consumer task to write on UART
