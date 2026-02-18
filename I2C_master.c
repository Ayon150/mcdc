// Master I2C Communication Protocol

// List of Register Declarations
#define RCC_BASE   (0x40023800)
#define GPIOA_BASE (0x40020000)
#define GPIOB_BASE (0x40020400)
#define I2C1_BASE  (0x40005400)
#define TIM2_BASE  (0x40000000)

#define RCC_AHB1ENR (*(volatile unsigned int *)(RCC_BASE + 0x30))
#define RCC_APB1ENR (*(volatile unsigned int *)(RCC_BASE + 0x40))

#define GPIOA_MODER (*(volatile unsigned int *)(GPIOA_BASE + 0x00))
#define GPIOA_OTYPER (*(volatile unsigned int *)(GPIOA_BASE + 0x04))
#define GPIOA_ODR   (*(volatile unsigned int *)(GPIOA_BASE + 0x14))

#define GPIOB_MODER (*(volatile unsigned int *)(GPIOB_BASE + 0x00))
#define GPIOB_OTYPER (*(volatile unsigned int *)(GPIOB_BASE + 0x04))
#define GPIOB_PUPDR (*(volatile unsigned int *)(GPIOB_BASE + 0x0C))
#define GPIOB_AFRH  (*(volatile unsigned int *)(GPIOB_BASE + 0x24))

#define I2C1_CR1 (*(volatile unsigned int *)(I2C1_BASE + 0x00))
#define I2C1_CR2 (*(volatile unsigned int *)(I2C1_BASE + 0x04))
#define I2C1_CCR (*(volatile unsigned int *)(I2C1_BASE + 0x1C))
#define I2C1_TRISE (*(volatile unsigned int *)(I2C1_BASE + 0x20))
#define I2C1_SR1 (*(volatile unsigned int *)(I2C1_BASE + 0x14))
#define I2C1_SR2 (*(volatile unsigned int *)(I2C1_BASE + 0x18))
#define I2C1_DR  (*(volatile unsigned int *)(I2C1_BASE + 0x10))

#define TIM2_PSC (*(volatile unsigned int *)(TIM2_BASE + 0x28))
#define TIM2_ARR (*(volatile unsigned int *)(TIM2_BASE + 0x2C))
#define TIM2_CR1 (*(volatile unsigned int *)(TIM2_BASE + 0x00))
#define TIM2_SR  (*(volatile unsigned int *)(TIM2_BASE + 0x10))

void I2C1_Init();  // 3) STM32_Nucleo_64_boards_Manualcs Page: 1, 35


void I2C1_Send(char saddr, int n, char* str);


int main(void) {
	I2C1_Init();
	while(1) {
		I2C1_Send(0x12, 6, "CSE-RU");
	}
}



void I2C1_Init() {

	
	RCC_AHB1ENR |= (1<<1);

	
	GPIOB_MODER |= (1<<19);  // Set 1
	GPIOB_MODER &= ~(1<<18); // Set 0 or Reset
	GPIOB_MODER |= (1<<17);
	GPIOB_MODER &= ~(1<<16);

	
	GPIOB_OTYPER |= (1<<8);
	GPIOB_OTYPER |= (1<<9);

	GPIOB_PUPDR &= ~(1<<19);
	GPIOB_PUPDR |= (1<<18);
	GPIOB_PUPDR &= ~(1<<17);
	GPIOB_PUPDR |= (1<<16);

	
	GPIOB_AFRH &= ~(0xFF<<0);
	GPIOB_AFRH |= (1<<2);
	GPIOB_AFRH |= (1<<6);

	RCC_APB1ENR |= (1<<21);

	I2C1_CR1 |= (1<<15);

	I2C1_CR1 &= ~(1<<15);

	
	I2C1_CR2 |= (1<<4);



	I2C1_CCR = 80;

	I2C1_TRISE = 17;

	
	I2C1_CR1 |= (1<<0);

}

void I2C1_Send(char saddr, int n, char* str) {
	
	
	while(I2C1_SR2 & (1<<1)) {}

	
	I2C1_CR1 |= (1<<8);             // Set Start Condition
	while(!(I2C1_SR1 & (1<<0))) {}  // Then Check and Wait is it Set or Not


	// See I2C Message Packet Start->Address
	// 1) STM32f446xx_Reference - 785, 788, I2C Address Frame, 762,785 
	// 1) STM32f446xx_Reference - 785, 788 (ACK as Address Match)
	/*Send slave address + write (0) bit and wait for ACK -- 762 & 785*/
	I2C1_DR = (saddr<<1);  // Set 0 to bit 0 position (master write), automaticall done because of left shift
	while(!(I2C1_SR1 & (1<<1))) {}   // 1st Read SR1

	// 1) STM32f446xx_Reference - 788 
	/*Clear ADDR flag to ensure address is sent -- 788*/
	(void)I2C1_SR2;   // 2nd Read SR2 int temp = I2C1_SR2 will generate warning

	// Start Data Transfer Byte by Byte
	// 1) STM32f446xx_Reference - 762 
	// Check whether Data Register is Empty or Not.
	// 1) STM32f446xx_Reference - 785, 787

	for(int i = 0; i < n; i++) {

		/*Wait until transmitter is empty -- 785*/
		while(!(I2C1_SR1 & (1<<7))) {}

		/*Place the data to the data register one byte at time*/
		I2C1_DR = *str++;
	}

	// 1) STM32f446xx_Reference - 785, 787
	/*Wait until transfer finished --788*/
	while(!(I2C1_SR1 & (1<<2))) {}

	// 1) STM32f446xx_Reference - 780, 781
	/*Generate stop -- 780*/
	I2C1_CR1 |= (1<<9);
}

