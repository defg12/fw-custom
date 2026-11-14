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

void setupBoardHardwareTestOverrides();

static void customBoardInitHardware() {
    /* Configure SkyStar microSD slot pins for 4-bit SDIO (AF12) */
    palSetPadMode(GPIOC, 8,  PAL_MODE_ALTERNATE(12) | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_PUPDR_PULLUP);
    palSetPadMode(GPIOC, 9,  PAL_MODE_ALTERNATE(12) | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_PUPDR_PULLUP);
    palSetPadMode(GPIOC, 10, PAL_MODE_ALTERNATE(12) | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_PUPDR_PULLUP);
    palSetPadMode(GPIOC, 11, PAL_MODE_ALTERNATE(12) | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_PUPDR_PULLUP);
    palSetPadMode(GPIOC, 12, PAL_MODE_ALTERNATE(12) | PAL_STM32_OSPEED_HIGHEST);
    palSetPadMode(GPIOD, 2,  PAL_MODE_ALTERNATE(12) | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_PUPDR_PULLUP);
}

static void customBoardConfigOverrides() {
    /* Force SkyStar PRO REF3030 3.00V precision ADC reference */
    engineConfiguration->adcVcc = 3.0f;

    /* Enable SD Card via 4-bit SDIO */
    engineConfiguration->isSdCardEnabled = true;
    engineConfiguration->sdCardCsPin = Gpio::Unassigned;
    engineConfiguration->sdCardSpiDevice = SPI_NONE;
}

static void customBoardDefaultConfiguration() {
    /* 1. Clear All Default Injector & Ignition Pins First */
    for (size_t i = 0; i < efi::size(engineConfiguration->injectionPins); i++) {
        engineConfiguration->injectionPins[i] = Gpio::Unassigned;
    }
    for (size_t i = 0; i < efi::size(engineConfiguration->ignitionPins); i++) {
        engineConfiguration->ignitionPins[i] = Gpio::Unassigned;
    }

    /* 2. Crank & Cam Trigger Inputs (Using hardware timer pins PB5 and PB7) */
    engineConfiguration->triggerInputPins = Gpio::B5;         // Primary crank trigger
    engineConfiguration->triggerInputPins = Gpio::Unassigned; // Secondary crank trigger
    engineConfiguration->camInputs = Gpio::B7;                // Primary cam trigger

    /* 3. Injector Outputs (Cylinders 1-4) */
    engineConfiguration->injectionPins = Gpio::E9;
    engineConfiguration->injectionPins = Gpio::E11;
    engineConfiguration->injectionPins = Gpio::E13;
    engineConfiguration->injectionPins = Gpio::E14;

    /* 4. Ignition Coil Outputs (Cylinders 1-4) */
    engineConfiguration->ignitionPins = Gpio::D8;
    engineConfiguration->ignitionPins = Gpio::D9;
    engineConfiguration->ignitionPins = Gpio::D10;
    engineConfiguration->ignitionPins = Gpio::D11;

    /* 5. Auxiliary Outputs (Fuel Pump, Fan, Idle, Tach, Boost) */
    engineConfiguration->fuelPumpPin = Gpio::D12;
    engineConfiguration->fanPin = Gpio::D13;
    engineConfiguration->idle.solenoidPin = Gpio::D14;
    engineConfiguration->tachOutputPin = Gpio::D15;
    engineConfiguration->boostControlPin = Gpio::E7;

    /* 6. Analog Sensors (Strictly 0-3.0V max on SkyStar PRO) */
    engineConfiguration->clt.adcChannel = EFI_ADC_0;        // PA0
    engineConfiguration->tps1_1AdcChannel = EFI_ADC_1;      // PA1
    engineConfiguration->oilPressure.hwChannel = EFI_ADC_2; // PA2
    engineConfiguration->map.sensor.hwChannel = EFI_ADC_3;  // PA3
    engineConfiguration->vbattAdcChannel = EFI_ADC_8;       // PB0
    engineConfiguration->iat.adcChannel = EFI_ADC_15;       // PC5

    /* 7. Analog Scaling, Oil Pressure & Thermistor Pull-Ups */
    engineConfiguration->adcVcc = 3.0f;
    engineConfiguration->analogInputDividerCoefficient = 1.6667f;
    engineConfiguration->vbattDividerCoeff = 6.0f;
    engineConfiguration->clt.config.bias_resistor = 2490;
    engineConfiguration->iat.config.bias_resistor = 2490;

    engineConfiguration->oilPressure.v1 = 0.30f;
    engineConfiguration->oilPressure.value1 = 0.0f;
    engineConfiguration->oilPressure.v2 = 2.70f;
    engineConfiguration->oilPressure.value2 = 689.48f;

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
    setupBoardHardwareTestOverrides();
    custom_board_InitHardware = customBoardInitHardware;
    custom_board_ConfigOverrides = customBoardConfigOverrides;
    custom_board_DefaultConfiguration = customBoardDefaultConfiguration;
}
