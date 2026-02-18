//uart transmeter

#define RCC_BASE (0x40023800)
#define GPIOA_BASE (0x40020000)
#define UART_BASE (0x40011000)



#define RCC_AHB1ENR (*(volatile unsigned int*)( RCC_BASE + 0x30))
#define GPIOA_MODER (*(volatile unsigned int*)(GPIOA_BASE + 0))
#define GPIOA_AFRH (*(volatile unsigned int*)(GPIOA_BASE + 0x24))
#define RCC_APB2ENR (*(volatile unsigned int*)(RCC_BASE + 0x44))
#define UART1_BRR (*(volatile unsigned int*)(UART_BASE + 0x08))
#define UART1_CR1 (*(volatile unsigned int*)(UART_BASE + 0x0C))
#define UART1_SR (*(volatile unsigned int*)(UART_BASE + 0))
#define UART1_DR (*(volatile unsigned int*)(UART_BASE + 0x04))


#define SYSCLK 16000000U
#define BAUD_RATE 9600U
void UART1_INIT();
void SEND(int n, char *str);

int main(){
	UART1_INIT();
	while(1){
		SEND(6, "CSE_RU");
	}
}

void UART1_INIT(){
	//Enable clock
	RCC_AHB1ENR |= (1<<0);

	// Alternate mode enable
	GPIOA_MODER &=~(1<<18);
	GPIOA_MODER |=(1<<19);

	//Introduce UART1 and for that set AF7

	GPIOA_AFRH &=~(0xF<<4);
	GPIOA_AFRH |=(0x7<<4);

	// Enable APB2
	RCC_APB2ENR|=(1<<4);

	//SET BAUD RATE
	UART1_BRR = ((SYSCLK + (BAUD_RATE/2))/BAUD_RATE);

	UART1_CR1 |= (1<<3);//TRANSMITER ENALE

	UART1_CR1 |=(1<<13); //UART_ENABLE

}

void SEND(int n, char* str){

	for(int i=0;i<n;i++){
		while(!(UART1_SR & (1<<7))){}
		UART1_DR = *str++;
	}

}
