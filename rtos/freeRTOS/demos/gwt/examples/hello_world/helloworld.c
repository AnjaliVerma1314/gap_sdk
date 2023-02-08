/**
 * Hello world example running on GWT processors(GAP8, GAP9) using single core.
 */

/* Modifying for MPU funuctionality */
/* PMSIS includes */
#include "pmsis.h"

/* Variables from Linker Script */

//Task 1 symbols
extern task_one_code_start;
extern task_one_code_end;
extern task_one_data_start; 
extern task_one_data_end;

//Task 2 symbols
extern task_two_code_start;
extern task_two_code_end;
extern task_two_data_start; 
extern task_two_data_end;

extern TaskHandle_t maintask;

void Task1Entry(void * pvtest);

void Task2Entry(void * pvtest);

/* Variables used. */
PI_L2 char hello[20];

 TASK1_DATA  uint32_t testvar = 0; 
 TASK1_DATA uint8_t ucParameterToPass = 0;
 TASK1_DATA StackType_t StackArray[1000];
 TASK1_DATA  StaticTask_t taskTCB;
 TASK1_DATA  TaskHandle_t xHandle = NULL;

TASK2_DATA uint32_t maintask2 = 0;
TASK2_DATA uint32_t testvar2 = 0; 
TASK2_DATA uint8_t ucParameterToPass2 = 0;
TASK2_DATA StackType_t StackArray2[1000];
TASK2_DATA StaticTask_t taskTCB2;
TASK2_DATA TaskHandle_t xHandle2 = NULL;

/*************** Machine mode task ***************/
void helloworld(void) // 'main' task entry function
{
      uint32_t task1var = 0, task2var = 0;
      
      maintask2 = maintask;
            
      // Suspend Self        
     vTaskSuspend(0); /* After this call, the control goes to Task1. Control comes back 
      			//to this task when it is resumed by Task1 */
 		
 	// Suspend Task1 
     vTaskSuspend(xHandle); 
               
        // Suspend Self  
     vTaskSuspend(0); /* After this call, the control goes to Task2. Control comes back 
      			//to this task when it is resumed by Task2 */

       task1var = testvar;
       task2var = testvar2;
       
       GAP_MPU_Disable();
      
	printf("\n testvar %d\n", task1var);
  	printf("\n testvar2 %d\n", task2var);

  
      pmsis_exit(0);;
}

/*************** User mode task ***************/
TASK1_CODE void Task1Entry(void * pvtest)  // 'Task1' entry function 
{
		
	testvar = 7;
	
	uint32_t value = 0;
	uint32_t count = 0;
	
	//When iteration count is 1000, application hangs.
	//When iteration count is 100, it works fine
	for(count = 0; count < 100; count++)
	{
		value = value + 1;
	}
	
	
	MPU_vTaskResume(maintask);
	
	/* The control does not reach here as 'main' task is at higher priority */

}

/*************** User mode task ***************/
TASK2_CODE void Task2Entry(void * pvtest)
{
	testvar2 = 13;

	MPU_vTaskResume(maintask2);
	
	/* The control does not reach here as 'main' task is at higher priority */
}

/*************** Program Entry ***************/
int main(void)
{
	printf("\n\n\t *** FreeRTOS HelloWorld *** \n\n");
	
	#if ( portUSING_MPU_WRAPPERS == 1 )
	//Create two user mode tasks
	
	uint32_t rule = 0;
	uint32_t mem_base = 0;
	uint32_t end = 0;
	uint32_t start = 0;
	uint32_t size = 0;
		
	TaskParameters_t Task1Param, Task2Param;
	char name1[6] = "Task1";
	char name2[6] = "Task2";
	
	Task1Param.pvTaskCode = Task1Entry;
	Task1Param.pcName = &name1[0];
	Task1Param.usStackDepth = 1000;
	Task1Param.pvParameters = &ucParameterToPass;
	Task1Param.uxPriority = 0x2;
	Task1Param.puxStackBuffer = &StackArray[0];
	Task1Param.pxTaskBuffer = &taskTCB;
	
	Task2Param.pvTaskCode = Task2Entry;
	Task2Param.pcName = &name2[0];
	Task2Param.usStackDepth = 1000;
	Task2Param.pvParameters = &ucParameterToPass2;
	Task2Param.uxPriority = 0x1;
	Task2Param.puxStackBuffer = &StackArray2[0];
	Task2Param.pxTaskBuffer = &taskTCB2;
	
	//Form MPU rules for Task1 and Task2
	mem_base = ((uint32_t)(&task_one_code_start)- 0x1c000000);
	end = (uint32_t)(&task_one_data_end);
	start = (uint32_t)(&task_one_code_start);
	size = ((end - start) >> 6);
	rule = GAP_MPU_RULE(GAP_MPU_L2_L2_AREA, mem_base, size); 
	printf("\n T1 : 0x%x \n", rule);
	Task1Param.xRegions[0].ulParameters = rule;
	mem_base = ((uint32_t)(&task_two_code_start)- 0x1c000000);
      	size = ((uint32_t)(&task_two_data_end) - (uint32_t)(&task_two_code_start)) >> 6;		
	rule = GAP_MPU_RULE(GAP_MPU_L2_L2_AREA, mem_base, size); 
	Task2Param.xRegions[0].ulParameters = rule;
	printf("\n T2 : 0x%x \n", rule);
	
	xTaskCreateRestrictedStatic(&Task1Param, &xHandle);
	xTaskCreateRestrictedStatic(&Task2Param, &xHandle2);
	
	//printf("\n H1 : 0x%x \n", xHandle);
	//printf("\n H2 : 0x%x \n", xHandle2);
	
	//Enable MPU
	//GAP_MPU_Enable(1);
		
	#endif
	return pmsis_kickoff((void *) helloworld); //Creates 'main' task and starts the scheduler
}


