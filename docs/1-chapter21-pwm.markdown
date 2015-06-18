# PWM实验 #

PWM（Pulse Width Modulation）即脉冲宽度调制的缩写，简称脉宽调制，是利用微处理器的数字输出来对模拟电路进行控制的一种非常有效的技术。 

## 实验原理与基础知识 ##

STM32F4的定时器除了TIM6和TIM7其它都可以用来产生PWM输出。其中高级定时器TIM1和TIM8可以同时产生多达7路的PWM输出。而通用定时器也能同时产生多达4路的PWM输出。

本案例使用TIM4的CH3产生一路PWM输出，通过改变占空比实现对D3亮度的控制。     

## 电路设计 ##

使用指示灯D3，复用功能可设置使用TIM4_CH3

![](img/chapter0/pwm_sch.png) 

## 库函数和数据结构 ##

使能TIM4和GPIO时钟，配置PB8选择复用功能AF8（TIM4）输出：

        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE); //TIM4 时钟使能       
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); //使能PB时钟 
        
初始化TIM14,设置TIM4的ARR和PSC等参数： 

        //设置ARR和PSC两个寄存器的值来控制输出PWM的周期。
        TIM_TimeBaseStructure.TIM_Prescaler=psc;    //定时器分频 
        TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式 
        TIM_TimeBaseStructure.TIM_Period=arr;      //自动重装载值 
        TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
        TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);//初始化定时器4
        
设置TIM4_CH3的PWM模式，使能TIM4的CH3输出：

        TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //PWM调制模式1 
        TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能 
        TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //输出极性低 
        TIM_OC3Init(TIM4, &TIM_OCInitStructure);    //初始化外设TIM4
        TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);    //使能预装载寄存器 
        TIM_ARRPreloadConfig(TIM4,ENABLE);//ARPE使能   
        TIM_Cmd(TIM4, ENABLE);    //使能TIM4  
        
设置占空比，通过修改TIM4_CCR1来控制占空比，PWM开始输出的占空比和频率都是固定的，而通过修改TIM4_CCR3则可以控制CH3的输出占空比，继而控制D3的亮度。 

        TIM_SetCompare3(TIM4,led0pwmval);  //修改比较值，修改占空比 

## 软件设计、开发 ##

1) 初始化定时器PWM模式 

        //arr：自动重装值  psc：时钟预分频数 
        void timer_pwm_init(u32 arr,u32 psc) 
        {                   
            GPIO_InitTypeDef GPIO_InitStructure; 
            TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure; 
            TIM_OCInitTypeDef    TIM_OCInitStructure; 

            RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE); //TIM4 时钟使能      
            RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); //使能PB时钟 

            GPIO_PinAFConfig(GPIOB,GPIO_PinSource8,GPIO_AF_TIM4); //PB8复用为TIM4 
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;                       //GPIOB8   
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;            //复用功能 
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;  //速度50MHz 
            GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;            //推挽复用输出 
            GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                //上拉 
            GPIO_Init(GPIOB,&GPIO_InitStructure);                            //初始化PB8 

            TIM_TimeBaseStructure.TIM_Prescaler=psc;    //定时器分频 
            TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式 
            TIM_TimeBaseStructure.TIM_Period=arr;      //自动重装载值 
            TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
            TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);//初始化定时器4

            //初始化TIM4 Channel3 PWM模式    
            TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //PWM调制模式1 
            TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能 
            TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //输出极性低 
            TIM_OC3Init(TIM4, &TIM_OCInitStructure);    //初始化外设TIM4
            TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);    //使能预装载寄存器 
            TIM_ARRPreloadConfig(TIM4,ENABLE);//ARPE使能   
            TIM_Cmd(TIM4, ENABLE);    //使能TIM4    

            //高级定时器，使能主输出，普通定时器不用加
            TIM_CtrlPWMOutputs(TIM4, ENABLE); 
        }    
        
        
2） 编写主函数 

        int main()
        {
            u16 led0pwmval=0;       
            u8 dir=1; 

            uart_config();
            timer_pwm_init(500-1,84-1);  
            /*定时器4时钟为84M，分频系数为84，所以计数频率 
            为84M/84=1Mhz，重装载值500，所以PWM频率为:1M/500=2Khz,脉宽周期为：1/2000=0.5ms*/
            while(1)    
            { 
                delay_ms(10);    
                if(dir)
                    led0pwmval++;//dir==1 led0pwmval 递增 
                else 
                    led0pwmval--; //dir==0 led0pwmval递减   
                if(led0pwmval>300){
                    dir=0;//led0pwmval 到达300后，方向为递减 
                }
                if(led0pwmval==0){
                    dir=1;  //led0pwmval 递减到0后，方向改为递增 
                }
                TIM_SetCompare3(TIM4,led0pwmval);  //修改比较值，修改占空比 
            } 
        }       
        
    
## 软件下载、测试验证 ##

