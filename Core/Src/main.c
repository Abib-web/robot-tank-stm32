/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "image.h"
#include "LCD_Test.h"
#include "LCD_2inch4.h"
#include "DEV_Config.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
/* Fonction pour ajuster la vitesse d'un moteur via PWM */
void Moteur1_SetSpeed(uint16_t speed, uint8_t direction);
void Moteur2_SetSpeed(uint16_t speed, uint8_t direction);

/* Fonction pour configurer les directions des moteurs */

/* Fonction pour les mouvements spécifiques */
void VirageGauche(uint16_t speed);
void VirageDroite(uint16_t speed);
void ArreterMoteurs(void);
void MarcheAvant(uint16_t speed);
uint8_t ReadDipSwitch(void);
void MarcheArriere(uint16_t speed);
void CalculerVitesse(void);
void LigneDroite(void);
void Cercle();
void Carre();
void EtapeUn(void);
void EtapeDeux(void);
void TournerAngle90();
void TournerAngle180();
void CommandeManuelle();
void ConfigI2C();
void TwoMoteurSpeed(uint16_t motor1, uint16_t motor2);
void ReadNUNCHUCK();
void Display_RTC_TimeDate(void);
void AfficherHeureDate(void);
void AfficherModeEtVitesse(void);
uint16_t CalculerVitesseMoteur(int impulsions);
void displayName(void);
void intLCD(void);
void updateNunchuk(void);
// Définitions pour l'adresse et le nombre de données
#define NUNCHUK_ADDRESS (0x52 << 1)
#define DATA_LENGTH 6
#define FORWARD 1
#define BACKWARD 2
#define STOP 3

/* USER CODE END 0 */
volatile int32_t impulsions_moteur1 = 0;
volatile int32_t impulsions_moteur2 = 0;
volatile int32_t impulsions_moteurv1 = 0;
volatile int32_t impulsions_moteurv2 = 0;

uint8_t init[2] = {0xF0, 0x55};  // Commande d'initialisation du Nunchuk
uint8_t data[6] = {0};
uint8_t joystickX = 0;
uint8_t joystickY =0;
uint8_t buttonC = 0;
uint8_t buttonZ = 0;

volatile uint8_t mode = 0;
char mode_vehicule[10] = "MANUEL";
volatile uint8_t isPaused = 0;
/**
  * @brief  The application entry point.
  * @retval int
  */
int moyenne = 0;
volatile uint8_t state0 = 0; // 0 = Idle, 1 = Test en cours, etc.
volatile uint8_t state1 = 0;
volatile uint8_t state2 = 0;
int test_config =0;
uint8_t* donnees;
uint8_t command = 0x00;
/* USER CODE END PFP */
uint8_t vitesse_gauche=0;
uint8_t vitesse_droite=0;
 uint8_t affichedata =0;
 uint8_t mode_commande = 0;
 uint8_t test =0;
 volatile uint8_t previous_mode = 0xFF;
 uint16_t MAX_PWM = 999;
/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
       RTC_TimeTypeDef sTime;
       RTC_DateTypeDef sDate;
/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI1_Init();
  MX_TIM3_Init();
  MX_TIM6_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  MX_TIM2_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
  /* USER CODE END 2 */

  /* USER CODE END 2 */
   HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
   HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
   HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
   HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
   HAL_TIM_Base_Start_IT(&htim6);

   ArreterMoteurs();
   intLCD();
   ConfigI2C();
   /* Infinite loop */
   /* USER CODE BEGIN WHILE */
 	/*	*/

   displayName();
   while (1) {


       // Lire le mode uniquement si nécessaire
       uint8_t current_mode = ReadDipSwitch();
       if (current_mode != previous_mode) {
           mode = current_mode;
           previous_mode = current_mode;
       }

       // Exécuter la logique en fonction du mode
       if (mode_commande == 0) {
           CommandeManuelle(); // Mode manuel
       } else {
    	   updateNunchuk();
    	   if (isPaused) {
    	              ArreterMoteurs();
    	              HAL_Delay(100);
    	              continue;
    	          }else{
    	        	  EtapeDeux(); // Mode automatique
    	          }

       }

       // Affichage des données
       AfficherHeureDate();
       AfficherModeEtVitesse();
   }

  /* USER CODE END 3 */
}

void intLCD(void){
	DEV_Module_Init();

	   	LCD_2IN4_Init();
	   	LCD_2IN4_Clear(WHITE);

	     Paint_NewImage(LCD_2IN4_WIDTH, LCD_2IN4_HEIGHT, ROTATE_90, WHITE);

	   	Paint_SetClearFuntion(LCD_2IN4_Clear);
	   	Paint_SetDisplayFuntion(LCD_2IN4_DrawPaint);
	     Paint_Clear(WHITE);
	     DEV_Delay_ms(1000);
}
void AfficherHeureDate(void) {
    static char previousBuffer[30] = {0}; // Pour stocker la dernière chaîne affichée

    // Lire l'heure et la date
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);


    // Formater l'heure et la date
    char buffer[30];
    snprintf(buffer, sizeof(buffer), "%02d:%02d %02d/%02d/20%02d",
             sTime.Hours, sTime.Minutes,
             sDate.Date, sDate.Month, sDate.Year);

    // Afficher uniquement si le contenu a changé
    if (strcmp(buffer, previousBuffer) != 0) {
        Paint_DrawString_EN(10, 10, buffer, &Font20, BLACK, WHITE);
        strncpy(previousBuffer, buffer, sizeof(previousBuffer));
    }
}


void displayName(void){
		Paint_SetRotate(ROTATE_90);
	    Paint_SetMirroring(MIRROR_HORIZONTAL);
	    // Définir les dimensions de l'écran pour le centrage

	    // Calculer les positions
	    int posX = 10;
	    int posY = 50;

	    // Afficher le texte "UQTR" centré avec un fond vert
	    Paint_DrawString_EN(posX, posY, "Mode :", &Font24, BLACK, RED);
	    Paint_DrawString_EN(posX, posY+20, "V.D :", &Font24, BLACK, BLUE);
	    Paint_DrawString_EN(posX, posY+40, "V.G :", &Font24, BLACK, BLUE);
	    Paint_DrawString_EN(posX, posY+60, "Ratio V:", &Font24, BLACK, BLUE);
}
void AfficherModeEtVitesse(void) {
	Paint_SetRotate(ROTATE_90);
	Paint_SetMirroring(MIRROR_HORIZONTAL);
    Paint_DrawString_EN(115, 50, mode_vehicule, &Font16, BLACK, WHITE);
        char buffer0[20];
        snprintf(buffer0, sizeof(buffer0), "%d RPM", vitesse_gauche);
        Paint_DrawString_EN(100, 75, buffer0, &Font16, BLACK, WHITE);

        char buffer1[20];
        snprintf(buffer1, sizeof(buffer1), "%d RPM", vitesse_droite);
        Paint_DrawString_EN(110, 90, buffer1, &Font16, BLACK, WHITE);

}



uint16_t CalculerVitesseMoteur(int impulsions) {
    // Exemple : Calculer la vitesse à partir des impulsions
    // Facteur à ajuster selon le capteur
    const float facteur_conversion = 60.0 / 20; // 20 impulsions par tour
    uint16_t vitesse = impulsions * facteur_conversion;
    impulsions = 0; // Réinitialiser le compteur
    return vitesse;
}

void updateNunchuk(){
    // Extraire les valeurs du joystick et des boutons
    joystickX = data[0];
    joystickY = data[1];
    buttonC = !(data[5] & 0x02);
    buttonZ = !(data[5] & 0x01);
    if (buttonC) {
         mode_commande = !mode_commande; // Bascule entre manuel (0) et automatique (1)
         //HAL_Delay(200);
    }
}
void CommandeManuelle() {
    // Lire les données du Nunchuck
    ReadNUNCHUCK();
    updateNunchuk();
    static uint8_t commande_active = 0;

    strncpy(mode_vehicule, mode_commande ? "AUTO" : "MANUEL", sizeof(mode_vehicule)); // Met à jour l'affichage
    // Changer le mode de vitesse avec le bouton Z
        static uint8_t mode_vitesse = 0;
        if (buttonZ) {
            mode_vitesse = (mode_vitesse + 1) % 3;
            HAL_Delay(200);
        }
        // Déterminer le rapport cyclique maximal pour le mode courant

           switch (mode_vitesse) {
               case 0: MAX_PWM = 333; break; // 33% de la vitesse maximale
               case 1: MAX_PWM = 666; break; // 66% de la vitesse maximale
               case 2: MAX_PWM = 999; break; // 100% de la vitesse maximale
           }
        // Déterminer le rapport cyclique maximal pour le mode courant
        test =1;
        // Commandes en fonction des joysticks
        if (joystickY > 140) { // Avancer
            if (joystickX < 115) { // Avancer à gauche
                Moteur1_SetSpeed(MAX_PWM, FORWARD);
                Moteur2_SetSpeed(MAX_PWM / 2, FORWARD);
            } else if (joystickX > 140) { // Avancer à droite
                Moteur1_SetSpeed(MAX_PWM / 2, FORWARD);
                Moteur2_SetSpeed(MAX_PWM, FORWARD);
            } else {
                MarcheAvant(MAX_PWM);
            }
        } else if (joystickY < 115) { // Reculer
            if (joystickX < 115) { // Reculer à gauche
                Moteur1_SetSpeed(MAX_PWM, BACKWARD);
                Moteur2_SetSpeed(MAX_PWM / 2, BACKWARD);
            } else if (joystickX > 140) { // Reculer à droite
                Moteur1_SetSpeed(MAX_PWM / 2, BACKWARD);
                Moteur2_SetSpeed(MAX_PWM, BACKWARD);
            } else {
                MarcheArriere(MAX_PWM);
            }
        }else if (joystickX < 115) { // Virage sur place à gauche
        	Moteur1_SetSpeed(MAX_PWM , FORWARD); // Moteur gauche avance
        	Moteur2_SetSpeed(MAX_PWM , BACKWARD); // Moteur droit recule

        } else if (joystickX > 140) { // Virage sur place à droite
            Moteur1_SetSpeed(MAX_PWM , BACKWARD); // Moteur gauche recule
            Moteur2_SetSpeed(MAX_PWM , FORWARD); // Moteur droit avance
        } else { // Arrêt (joystick au centre)
            ArreterMoteurs();
        }
}





void ConfigI2C(){
	    // Initialisation du Nunchuk
	    if (HAL_I2C_Master_Transmit(&hi2c1, NUNCHUK_ADDRESS, init, 2, HAL_MAX_DELAY) != HAL_OK) {
	        Error_Handler();
	    }else{
	    	test_config =1;
	    }
	    HAL_Delay(100);

	    // Envoi de la commande 0x00 pour démarrer la communication
	    uint8_t command = 0x00;
	    if (HAL_I2C_Master_Transmit(&hi2c1, NUNCHUK_ADDRESS, &command, 1, HAL_MAX_DELAY) != HAL_OK) {

	        Error_Handler();
	    }
	    HAL_Delay(10);
}

/* Fonction ReadNUNCHUCK corrigée */
void ReadNUNCHUCK() {
    HAL_StatusTypeDef status;

    // Transmission de la commande
    status = HAL_I2C_Master_Transmit(&hi2c1, NUNCHUK_ADDRESS, 0x00, 1, HAL_MAX_DELAY);
    if (status != HAL_OK) {
        Error_Handler(); // Gestion des erreurs
    }

    // Attente pour la réponse du périphérique
    HAL_Delay(10);

    // Réception des données dans le tableau global `data`
    status = HAL_I2C_Master_Receive(&hi2c1, NUNCHUK_ADDRESS, data, DATA_LENGTH, HAL_MAX_DELAY);
    if (status != HAL_OK) {
        Error_Handler(); // Gestion des erreurs
    }
}



void TournerAngle90() {
    int impulsions_cible = 105;

    // Réinitialisation des impulsions
    impulsions_moteur1 = 0;
    impulsions_moteur2 = 0;

    // Rotation : une roue avance, l'autre recule
    Moteur1_SetSpeed(500,1);
    Moteur2_SetSpeed(520, 2);

    // Boucle jusqu'à atteindre l'angle souhaité
    while (impulsions_moteur1 < impulsions_cible && impulsions_moteur2 < impulsions_cible) {
    }

    // Arrêt des moteurs
    ArreterMoteurs();
}

void TournerAngle180() {
    int impulsions_cible = 205;

    // Réinitialisation des impulsions
    impulsions_moteur1 = 0;
    impulsions_moteur2 = 0;

    // Rotation : une roue avance, l'autre recule
    Moteur1_SetSpeed(500,FORWARD);
    Moteur2_SetSpeed(520,BACKWARD);

    // Boucle jusqu'à atteindre l'angle souhaité
    while (impulsions_moteur1 < impulsions_cible && impulsions_moteur2 < impulsions_cible) {
    }

    // Arrêt des moteurs
    ArreterMoteurs();
}


void LigneDroite() {
	int target_impulsions = 600;//456;
	    impulsions_moteur1 = 0;
	    impulsions_moteur2 = 0;

	    // Démarre les moteurs
	    MarcheAvant(500);

	    while (impulsions_moteur1 < target_impulsions && impulsions_moteur2 < target_impulsions) {
	    }

	    //
	    ArreterMoteurs();
}




void LigneDroiteRetour() {
	LigneDroite();
	TournerAngle180();
	LigneDroite();
}
void Cercle() {
	uint16_t MAX_PWM =999;
    impulsions_moteur1 = 0;
    impulsions_moteur2 = 0;

    Moteur1_SetSpeed(MAX_PWM, FORWARD);
    Moteur2_SetSpeed(MAX_PWM / 2, FORWARD);

    while (impulsions_moteur1 < 2000) {}
    ArreterMoteurs();
}

void Carre() {
    for (int i = 0; i < 4; i++) {
        LigneDroite();
        TournerAngle90();
    }
    ArreterMoteurs();
}

uint8_t ReadDipSwitch(void) {
    uint8_t mode = 0;

    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) == GPIO_PIN_RESET) { // IN1
        mode |= (1 << 0); // Bit 0
        state0 = 1;
    } else {
        state0 = 0;
    }

    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4) == GPIO_PIN_RESET) { // IN2
        mode |= (1 << 1); // Bit 1
        state1 = 1;
    } else {
        state1 = 0;
    }

    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == GPIO_PIN_RESET) { // IN3
        mode |= (1 << 2); // Bit 2
        state2 = 1;
    } else {
        state2 = 0;
    }

    return mode;
}



void Moteur2_SetSpeed(uint16_t speed, uint8_t direction) {

        if(direction == FORWARD){ // avance
        	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 0);      // Aucun signal
        	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, speed);
        }else if(direction == BACKWARD){ // recule
        	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3,speed);
            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, 0);
        } else if(direction >=3){
        	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 0);  // PWM pour avancer
        	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, 0);
        }
}


void Moteur1_SetSpeed(uint16_t speed, uint8_t direction) {
		if(direction == 1){
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, speed);  // PWM pour avancer
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);
		}else if(direction == 2){
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0);  // PWM pour avancer
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, speed);
		} else if(direction >=3){
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0);  // PWM pour avancer
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);
		}

}



void MarcheArriere(uint16_t speed) {
    Moteur1_SetSpeed(speed, 2); // Moteur 1 recule
    Moteur2_SetSpeed(speed, 2); // Moteur 2 recule
}



void VirageDroite(uint16_t speed) {
    Moteur1_SetSpeed(speed, 1); // Moteur 1 avance plus lentement
    Moteur2_SetSpeed(speed/ 2, 1);   // Moteur 2 avance
}


void VirageGauche(uint16_t speed) {
    Moteur1_SetSpeed(speed / 2, 1); // Moteur 1 avance plus lentement
    Moteur2_SetSpeed(speed, 1);   // Moteur 2 avance
}


void ArreterMoteurs(void) {
    Moteur1_SetSpeed(0,3);
    Moteur2_SetSpeed(0,3);
}

void MarcheAvant(uint16_t speed) {
    Moteur1_SetSpeed(speed+60, FORWARD);  // Moteur 1 avance
    Moteur2_SetSpeed(speed, FORWARD);  // Moteur 2 avance
}

void EtapeUn(void) {
    // Étape 1 : Marche avant
    MarcheAvant(1000);  // PWM à 100% pour tester
    HAL_Delay(3000);
    // Étape 5 : Virage gauche
    VirageGauche(500);  // PWM à 50%
    HAL_Delay(1500);

    // Étape 6 : Virage droite
    VirageDroite(500);  // PWM à 50%
    HAL_Delay(1500);

    // Étape 7 : Marche arrière
    MarcheArriere(1000);  // PWM à 100%
    HAL_Delay(3000);

}

void EtapeDeux(void) {
    switch (mode) {
        case 1: // Ligne droite et retour
            if (state0 == 1) {
                LigneDroiteRetour();
                ArreterMoteurs();
                state0 = 0;
                mode = 0;
            }
            break;
        case 2: // Cercle
            if (state1 == 1) {
                Cercle();
                ArreterMoteurs();
                state1 = 0;
                mode = 0;
            }
            break;
        case 4: // Carré
            if (state2 == 1) {
                Carre();
                ArreterMoteurs();
                state2 = 0;
                mode = 0;
            }
            break;
        default:
            ArreterMoteurs();
            break;
    }
}


/* Callback pour gérer la pause avec le bouton bleu */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GPIO_PIN_13){
        isPaused = !isPaused; // Activer/désactiver la pause
    }
    if (GPIO_Pin == GPIO_PIN_4) { // Encoder Moteur 2 - Signal A
                impulsions_moteur2++;
                impulsions_moteurv2++;
            }
    if (GPIO_Pin == GPIO_PIN_0) { // Encoder Moteur 1 - Signal A
            impulsions_moteur1++;
            impulsions_moteurv1++;
        }

}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM6) { // Vérifier si l'interruption provient de TIM6
        // Calculer les vitesses
        vitesse_gauche = CalculerVitesseMoteur(impulsions_moteurv1);
        vitesse_droite = CalculerVitesseMoteur(impulsions_moteurv2);

        // Réinitialiser les compteurs d'impulsions
        impulsions_moteurv1 = 0;
        impulsions_moteurv2 = 0;
    }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_OFF;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 50;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
