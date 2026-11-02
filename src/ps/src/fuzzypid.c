#include "../include/fuzzypid.h"
#include "../include/pid.h"

#define NB   -3
#define NM	 -2
#define NS	 -1
#define ZO	 0
#define PS	 1
#define PM	 2
#define PB	 3

 //规则库，KP、KI的增量模糊规则表
static const float ruleKp[7][7]=
{
	NB,	NB,	NM,	NM,	NM,	PB,	PB,
	NB,	NM,	NS,	NS,	NS,	PM,	PB,
	PS,	PS,	PS,	ZO,	NS,	NS,	NS,
	NM,	NS,	ZO,	ZO,	ZO,	PS,	PM,
	NS,	NS,	NS,	ZO,	PS,	PS,	PS,
	NB,	PM,	PS,	PS,	PS,	PM,	PB,
	PB,	PB,	PM,	PM,	PM,	PB,	PB
};

static const float ruleKi[7][7]=
{
	PB,	PB,	PM,	PM,	PM,	NB,	NB,
	PB,	PM,	PS,	PS,	PS,	NM,	NB,
	NS,	NS,	NS,	ZO,	PS,	PS,	PS,
	PM,	PS,	ZO,	ZO,	ZO,	NS,	NM,
	PS,	PS,	PS,	ZO,	NS,	NS,	NS,
	PB,	NM,	NS,	NS,	NS,	NM,	NB,
	NB,	NB,	NM,	NM,	NM,	NB,	NB
};



 //初始化结构体参数
FUZZYPID FPID={
	0,0,0,0,0,0,
	0,0,1,1,1
};

//隶属度计算函数，这里隶属度函数我全部选择的三角函数
static void CalcMembership(float *ms,float qv,int * index)
{
 if((qv>=NB)&&(qv<NM))
 {
     index[0]=0;
     index[1]=1;
     ms[0]=(-2-qv)/(-2+3);
     ms[1]=(qv+3)/(-2+3);
 }
 else if((qv>=NM)&&(qv<NS))
 {
     index[0]=1;
     index[1]=2;
     ms[0]=-qv-1;
     ms[1]=qv+2;
 }
 else if((qv>=NS)&&(qv<ZO))
 {
     index[0]=2;
     index[1]=3;
     ms[0]=-qv;
     ms[1]=qv+1;
 }
 else if((qv>=ZO)&&(qv<PS))
 {
     index[0]=3;
     index[1]=4;
     ms[0]=-qv+1;
     ms[1]=qv;
 }
 else if((qv>=PS)&&(qv<PM))
 {
     index[0]=4;
     index[1]=5;
     ms[0]=-qv+2;
     ms[1]=qv-1;
 }
 else if((qv>=PM)&&(qv<=PB))
 {
     index[0]=5;
     index[1]=6;
     ms[0]=-qv+3;
     ms[1]=qv-2;
  }
}

//输入值的量化论域(-3->3)
//_Real_Value为理想值
static void LinearQuantization(FUZZYPID *vPID,float _Real_Value,float *qValue)
{
     float thisError;
     float deltaError;

     thisError=vPID->setVaule-_Real_Value;   //计算当前偏差
     deltaError=thisError-vPID->lasterror;   //计算偏差增量

//E和EC的量化
     qValue[0]=6.0*thisError/(vPID->maximum-vPID->minimum);
     qValue[1]=6.0*deltaError/2.0*(vPID->maximum-vPID->minimum);//ec是e的一半

}


//解模糊，重心法
static void FuzzyComputation (FUZZYPID *vPID,float _Real_Value)
{
	//量化值
     float qValue[2]={0,0};

     int indexE[2]={0,0};     //e在规则库中的索引
     float msE[2]={0,0};      //e的隶属度

     int indexEC[2]={0,0};    //ec在规则库中的索引
     float msEC[2]={0,0};      //ec的隶属度

   //pid增量值
     float pidvalue[2];   //只用了KP、KI

//量化
 LinearQuantization(vPID,_Real_Value,qValue);
//计算e的隶属度和索引
 CalcMembership(msE,qValue[0],indexE);
//计算ec的隶属度和索引
 CalcMembership(msEC,qValue[1],indexEC);

 //采用重心法计算pid增量值
 pidvalue[0]=msE[0]*(msEC[0]*ruleKp[indexE[0]][indexEC[0]]+msEC[1]*ruleKp[indexE[0]][indexEC[1]])
            +msE[1]*(msEC[0]*ruleKp[indexE[1]][indexEC[0]]+msEC[1]*ruleKp[indexE[1]][indexEC[1]]);
 pidvalue[1]=msE[0]*(msEC[0]*ruleKi[indexE[0]][indexEC[0]]+msEC[1]*ruleKi[indexE[0]][indexEC[1]])
            +msE[1]*(msEC[0]*ruleKi[indexE[1]][indexEC[0]]+msEC[1]*ruleKi[indexE[1]][indexEC[1]]);


//pid增量修正
 vPID->deta_kp=vPID->qKp*pidvalue[0];
 vPID->date_ki=vPID->qKi*pidvalue[1];


}


/*程序入口主函数*/
void Fuzzytrans(float _Set_Vaule,float _Measure_Vaule,float pre_Measure_Vaule)
{
	FPID.setVaule=_Set_Vaule;
	//上一次的偏差值
	FPID.lasterror=_Set_Vaule-pre_Measure_Vaule;

	//修正系数
	FPID.qKp=0.000067;
	FPID.qKi=0.0002;


	//输出量的最大最小值
	FPID.maximum = 11;//进行量化需要进行量化因子计算
	FPID.minimum = -79;
	FuzzyComputation(&FPID,_Measure_Vaule);//_Measure_Vaule是输入实际值，pre_Measure_Vaule是输入上一次值
}

