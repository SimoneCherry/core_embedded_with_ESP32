#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_err.h"

#include "driver/gpio.h"
#include "esp_intr_alloc.h"


#define INTR_PRIO ESP_INTR_FLAG_LEVEL1
gpio_num_t PINS[7] = {
    GPIO_NUM_19, //a
    GPIO_NUM_18, //b
    GPIO_NUM_23, //c    
    GPIO_NUM_21, //d
    GPIO_NUM_16, //e
    GPIO_NUM_4,  //f
    GPIO_NUM_17  //g
};

#define INPUT_PIN GPIO_NUM_22

volatile bool Obstraction_Detected = false;
/*************************
         a
      -------
     |       |
   f |   g   | b
      -------          7 Segment Display
     |       |
   e |       | c
      -------
         d
**************************/

int Segment_Arrays[10][7]={
      /* a,b,c,d,e,f,g  */
        {1,1,1,1,1,1,0}, // Zero
        {0,1,1,0,0,0,0}, // One
        {1,1,0,1,1,0,1}, // Two
        {1,1,1,1,0,0,1}, // Three
        {0,1,1,0,0,1,1}, // Four
        {1,0,1,1,0,1,1}, // Five
        {1,0,1,1,1,1,1}, // Six
        {1,1,1,0,0,0,0}, // Seven
        {1,1,1,1,1,1,1}, // Eight
        {1,1,1,0,0,1,1}  // Nine
};

int Bit_Map_Obstacle_Detected[7] = {1,1,1,0,1,1,1};
void IRAM_ATTR Obstacle_Detector (void * pvParameters){
        // While Loop Disabled beacuse function has bee nconfigured as Interrupt
        //while (1){
        Obstraction_Detected=!(gpio_get_level(INPUT_PIN));
        //printf("Obstacle Detected is %d \n", (int) Obstraction_Detected);
        //vTaskDelay(pdMS_TO_TICKS(500));
        //printf("Task B is Running");
        //}

}
void GPIO_7Segment_BitMapping(void * pvParameters){

 while (1){
        if (Obstraction_Detected==false){
                for (int digit=0 ; digit <10 && Obstraction_Detected==false;digit++)
                {
                printf("Digit is ..%d\n", digit);
                for(int Segments=0 ; Segments<7;Segments++){
                        gpio_set_level(PINS[Segments], (int) Segment_Arrays[digit][Segments]);        
                        vTaskDelay(pdMS_TO_TICKS(5));
                        if(Obstraction_Detected==true){
                                break;
                        }
                        printf("Obstacle Detected %d \n", (int)Obstraction_Detected);
                }
                vTaskDelay(pdMS_TO_TICKS(1000));
                }
                vTaskDelay(pdMS_TO_TICKS(500));
                printf("Task A is Running\n");
        }
        else{
               for(int Segments=0 ; Segments<7;Segments++){
                        gpio_set_level(PINS[Segments], (int) Bit_Map_Obstacle_Detected[Segments]);        
                        vTaskDelay(pdMS_TO_TICKS(20));
                }
                vTaskDelay(pdMS_TO_TICKS(800));
                for(int Segments=0 ; Segments<7;Segments++){
                        gpio_set_level(PINS[Segments], (int)0);        
                        vTaskDelay(pdMS_TO_TICKS(20));
                }
        }
 }
}

void GPIO_Init(void)
{
        for (int pin=0; pin<sizeof(PINS)/sizeof(PINS[0]); pin++){
                gpio_set_direction(PINS[pin], GPIO_MODE_OUTPUT);
                //gpio_set_level(PINS[pin], 1);
        }

        gpio_set_direction(INPUT_PIN, GPIO_MODE_INPUT);

        gpio_install_isr_service(INTR_PRIO);
        gpio_isr_handler_add(INPUT_PIN, Obstacle_Detector, NULL);
        gpio_set_intr_type(INPUT_PIN, GPIO_INTR_ANYEDGE);
        gpio_intr_enable(INPUT_PIN);
}

void app_main(void)
{        
        GPIO_Init();
        xTaskCreate( GPIO_7Segment_BitMapping,
                         "GPIO 7 Segment High Low Mapping",
                         2024,
                         NULL,
                         1,
                         NULL
                       );
        /*xTaskCreate( Obstacle_Detector,
                         "Detects Obstacle",
                         1024,
                         NULL,
                         10,
                         NULL
                       );*/
        


}

