#include "pch.h"
#include "board_overrides.h"

Gpio getCommsLedPin() {
	return Gpio::B2;
}

Gpio getRunningLedPin() {
	return Gpio::Unassigned;
}

Gpio getWarningLedPin() {
	return Gpio::Unassigned;
}

//void setupBoardHardwareTestOverrides();

static void customBoardInitHardware() {
    /* Keep external SPI flash chip disabled (PA4 /CS = HIGH) */
    efiSetPadMode("External Flash CS", Gpio::A4, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPad(GPIOA, 4);

    /* Configure SkyStar microSD slot pins for 4-bit SDIO (AF12) */
    palSetPadMode(GPIOC, 8,  PAL_MODE_ALTERNATE(12) | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_PUPDR_PULLUP);
    palSetPadMode(GPIOC, 9,  PAL_MODE_ALTERNATE(12) | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_PUPDR_PULLUP);
    palSetPadMode(GPIOC, 10, PAL_MODE_ALTERNATE(12) | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_PUPDR_PULLUP);
    palSetPadMode(GPIOC, 11, PAL_MODE_ALTERNATE(12) | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_PUPDR_PULLUP);
    palSetPadMode(GPIOC, 12, PAL_MODE_ALTERNATE(12) | PAL_STM32_OSPEED_HIGHEST);
    palSetPadMode(GPIOD, 2,  PAL_MODE_ALTERNATE(12) | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_PUPDR_PULLUP);
}

static void customBoardConfigOverrides() {
    /* Force SkyStar PRO REF3033 3.3V precision ADC reference */
    engineConfiguration->adcVcc = 3.3f;
    engineConfiguration->vbattAdcChannel = EFI_ADC_8; // PB0
	engineConfiguration->vbattDividerCoeff = 11.0f;
	
    /* Enable SD Card via 4-bit SDIO */
    engineConfiguration->isSdCardEnabled = true;
    engineConfiguration->sdCardCsPin = Gpio::Unassigned;
    engineConfiguration->sdCardSpiDevice = SPI_NONE;

	/* Hardcode PB5 (Cam / VR trigger input) on every boot */
    engineConfiguration->triggerInputPins[0] = Gpio::B5; // Primary crank trigger  (PB4)
    engineConfiguration->triggerInputPins[1] = Gpio::Unassigned;

    /* Hardcode ETB H-Bridge pins on every boot (Dir + PWM + Disable) */
    engineConfiguration->etbIo[0].directionPin1 = Gpio::D7; // ETB Direction (PD7)
    engineConfiguration->etbIo[0].controlPin = Gpio::C6;   // ETB PWM (PC6)
    engineConfiguration->etbIo[0].disablePin = Gpio::B6;    // ETB Disable (PB6)
    engineConfiguration->etb_use_two_wires = false;
}

static void setDefaultIgnitionPins() {
    for (size_t i = 0; i < efi::size(engineConfiguration->ignitionPins); i++) {
        engineConfiguration->ignitionPins[i] = Gpio::Unassigned;
    }

    engineConfiguration->ignitionPins[0] = Gpio::D12; // Coil 1 (PD12)
    engineConfiguration->ignitionPins[1] = Gpio::D13; // Coil 2 (PD13)
    engineConfiguration->ignitionPins[2] = Gpio::D14; // Coil 3 (PD14)
    engineConfiguration->ignitionPins[3] = Gpio::D15; // Coil 4 (PD15)
}

/* Helper: Clear and assign Injector outputs */
static void setDefaultInjectorPins() {
    for (size_t i = 0; i < efi::size(engineConfiguration->injectionPins); i++) {
        engineConfiguration->injectionPins[i] = Gpio::Unassigned;
    }

    engineConfiguration->injectionPins[0] = Gpio::E9; // Injector 1 (PE9)
    engineConfiguration->injectionPins[1] = Gpio::E11; // Injector 2 (PE11)
    engineConfiguration->injectionPins[2] = Gpio::E13; // Injector 3 (PE13)
    engineConfiguration->injectionPins[3] = Gpio::E14; // Injector 4 (PE14)
}

static void customBoardDefaultConfiguration() {
    setDefaultIgnitionPins();
    setDefaultInjectorPins();
	
	engineConfiguration->useMetricOnInterface = true;
	
    /* Crank & Cam Trigger Inputs */
    engineConfiguration->camInputs[0] = Gpio::B4;           // Primary cam trigger
	
    /* Analog Sensors (Strictly 0-3.3V max on SkyStar PRO) */
    engineConfiguration->clt.adcChannel = EFI_ADC_0;        // PA0
    engineConfiguration->tps1_1AdcChannel = EFI_ADC_1;      // PA1
    engineConfiguration->oilPressure.hwChannel = EFI_ADC_2; // PA2
    engineConfiguration->map.sensor.hwChannel = EFI_ADC_3;  // PA3
	engineConfiguration->auxLinear1.hwChannel = EFI_ADC_9;  //PB1
    engineConfiguration->auxTempSensor1.adcChannel = EFI_ADC_10; // PC0
	engineConfiguration->auxLinear2.hwChannel = EFI_ADC_11;
	engineConfiguration->auxTempSensor2.adcChannel = EFI_ADC_12; // PC2
	engineConfiguration->iat.adcChannel = EFI_ADC_15;       // PC5
    engineConfiguration->analogInputDividerCoefficient = (4.7f + 6.8f) / 6.8f; // 1.691176f
	
    /* Analog Scaling, Oil Pressure & Thermistor Pull-Ups */  
    engineConfiguration->clt.config.bias_resistor = 4700;
    engineConfiguration->iat.config.bias_resistor = 4700;
    engineConfiguration->auxTempSensor1.config.bias_resistor = 4700;
	engineConfiguration->auxTempSensor2.config.bias_resistor = 4700;
	
    engineConfiguration->oilPressure.v1 = 0.30f;
    engineConfiguration->oilPressure.value1 = 0.0f;
    engineConfiguration->oilPressure.v2 = 2.70f;
    engineConfiguration->oilPressure.value2 = 689.48f;

	engineConfiguration->auxLinear1.v1 = 0.50f;            // Sensor voltage at low end
    engineConfiguration->auxLinear1.value1 = 0.0f;         // Pressure in kPa at v1
    engineConfiguration->auxLinear1.v2 = 4.50f;            // Sensor voltage at high end
    engineConfiguration->auxLinear1.value2 = 1000.0f;      // Pressure in kPa at v2 (10 bar)

	engineConfiguration->auxLinear2.v1 = 0.50f;            // Sensor voltage at low end
    engineConfiguration->auxLinear2.value1 = 0.0f;         // Pressure in kPa at v1
    engineConfiguration->auxLinear2.v2 = 4.50f;            // Sensor voltage at high end
    engineConfiguration->auxLinear2.value2 = 1000.0f;      // Pressure in kPa at v2 (10 bar)
    
	/* 8. CAN Bus (CAN1 on PD0 / PD1, CAN2 disabled) */
    engineConfiguration->canRxPin = Gpio::D0;
    engineConfiguration->canTxPin = Gpio::D1;
    engineConfiguration->can2RxPin = Gpio::Unassigned;
    engineConfiguration->can2TxPin = Gpio::Unassigned;

    /* 9. Serial Configuration (USB OTG1 + USART2 on PD5/PD6, USART1 disabled) */
    engineConfiguration->binarySerialTxPin = Gpio::D5;
    engineConfiguration->binarySerialRxPin = Gpio::D6;
    engineConfiguration->uartConsoleSerialSpeed = 115200;
}

void setup_custom_board_overrides() {
    custom_board_InitHardware = customBoardInitHardware;
    custom_board_ConfigOverrides = customBoardConfigOverrides;
    custom_board_DefaultConfiguration = customBoardDefaultConfiguration;
}
