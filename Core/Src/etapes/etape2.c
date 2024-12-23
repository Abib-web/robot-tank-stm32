#include "image.h"
#include "LCD_2inch4.h"
#include "LCD_Test.h"
#include "DEV_Config.h"

void LCD_2in4_Etape()
{
    printf("LCD_2IN4_test Demo\r\n");
    DEV_Module_Init();

    printf("LCD_2IN4_ Init and Clear...\r\n");
    LCD_2IN4_Init();
    LCD_2IN4_Clear(GREEN);

    printf("Paint_NewImage\r\n");
    Paint_NewImage(LCD_2IN4_WIDTH, LCD_2IN4_HEIGHT, ROTATE_90, GREEN);

    printf("Set Clear and Display Funtion\r\n");
    Paint_SetClearFuntion(LCD_2IN4_Clear);
    Paint_SetDisplayFuntion(LCD_2IN4_DrawPaint);

    printf("Paint_Clear\r\n");
    Paint_Clear(GREEN);
    DEV_Delay_ms(1000);

    printf("Painting...\r\n");
    Paint_SetRotate(ROTATE_90); // Assurez-vous que le texte soit dans la bonne orientation
    Paint_SetMirroring(MIRROR_HORIZONTAL);
    // Définir les dimensions de l'écran pour le centrage
    int screenWidth = LCD_2IN4_HEIGHT; // largeur de l’écran en mode rotation
    int screenHeight = LCD_2IN4_WIDTH; // hauteur de l’écran en mode rotation

    // Définir les dimensions de la police utilisée (Font24)
    int textWidth = Font24.Width * 4;   // "UQTR" a 4 caractères
    int textHeight = Font24.Height;

    // Calculer les positions pour centrer le texte "UQTR"
    int posX = (screenWidth - textWidth) / 2;
    int posY = (screenHeight - textHeight) / 2;

    // Afficher le texte "UQTR" centré avec un fond vert
    Paint_DrawString_EN(posX, posY, "UQTR", &Font24, GREEN, YELLOW);

    DEV_Delay_ms(3000);

    printf("quit...\r\n");
    //DEV_Module_Exit();
}
