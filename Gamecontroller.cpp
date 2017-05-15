#include <iostream>
#include <ros/ros.h>
#include <stdlib.h>
#include "RoboCupGameControlData.h"
#include "Gamecontroller/Mydata.h"
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "RoboCupGameControlData.h"
#include "SPLCoachMessage.h"

using namespace std;

Gamecontroller::Mydata updateMydata(RoboCupGameControlData d);

int main(int argc,char** argv)
{

    ros::init(argc,argv,"publish_Gamecontroller_msg");

    ros::NodeHandle nh;

    ros::Publisher pub=nh.advertise<Gamecontroller::Mydata>("turtle1/cmd_vel",1000);

    srand(time(0));

    ros::Rate loop_rate(2);

    setvbuf(stdout, NULL, _IONBF, 0);

    fflush(stdout);

    // 绑定地址
    struct sockaddr_in addrto;

    bzero(&addrto, sizeof(struct sockaddr_in));

    addrto.sin_family = AF_INET;

    addrto.sin_addr.s_addr = htonl(INADDR_ANY);

    addrto.sin_port = htons(3838);

    int nlen=sizeof(addrto);

    // 广播地址
    struct sockaddr_in from;

    bzero(&from, sizeof(struct sockaddr_in));

    from.sin_family = AF_INET;

    from.sin_addr.s_addr = htonl(INADDR_ANY);

    from.sin_port = htons(3934);

    int sock = -1;

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        cout<<"socket error"<<endl;

        return false;
    }

    const int opt = 1;
    //设置该套接字为广播类型，
    int nb = 0;

    nb = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&opt, sizeof(opt));

    if(nb == -1)
    {
        cout<<"set socket error..."<<endl;

        return false;
    }

    if(bind(sock,(struct sockaddr *)&(addrto), sizeof(struct sockaddr_in)) == -1)
    {
        cout<<"bind error..."<<endl;

        return false;
    }

    int len = sizeof(sockaddr_in);

    char smsg[10000] = {};

    char* smsg2;

    while (ros::ok()){

        Gamecontroller::Mydata mydata;

        int ret=recvfrom(sock, smsg, 10000, 0, (struct sockaddr*)&from,(socklen_t*)&len);

        RoboCupGameControlData d = *(RoboCupGameControlData*)(smsg);

        mydata = updateMydata(d);

        pub.publish(mydata);

        ROS_INFO_STREAM("receiving msg:"<<" version = "<<mydata.Protocol_Version);

        string s = "RGrt0000";//后四字节为协议号/队号/球员号/message

        smsg2 = (char *)malloc(9*sizeof(s));

        s.copy(smsg2,9,0);

        smsg2[7]= '?';                              //在此替换为需要发出的message

        int ret2=sendto(sock, smsg2, strlen(smsg2), 0, (sockaddr*)&addrto, nlen);

        if(ret2<0)
        {
            cout<<"send error...."<<ret<<endl;
        }
        else
        {
            printf("ok ");
        }


        loop_rate.sleep();
    }
    return 0;
}

Gamecontroller::Mydata updateMydata(RoboCupGameControlData d){
    Gamecontroller::Mydata mydata;
    mydata.Msg_Header.resize(4);
    for (int i=0;i<4;i++){
	mydata.Msg_Header[i] = d.header[i];	
	}
    mydata.Protocol_Version = d.version;
    mydata.packerNumber = d.packetNumber;
    mydata.gameType = d.gameType;
    mydata.state = d.state;
    mydata.fistHalf = d.firstHalf;
    mydata.kickOffTeam = d.kickOffTeam;
    mydata.secondaryState = d.secondaryState;
    mydata.secondaryStateInfo.resize(4);
    for(int i=0;i<4;i++){
	mydata.secondaryStateInfo[i] = d.secondaryStateInfo[i];
	}
    mydata.dropInTeam = d.dropInTime;
    mydata.secsRemaining = d.secsRemaining;
    mydata.secondaryTime = d.secondaryTime;

    mydata.TI_teamNUmber = d.teams[0].teamNumber;
    mydata.TI_teamColour = d.teams[0].teamColour;
    mydata.TI_score = d.teams[0].score;
    mydata.TI_penaltyShot = d.teams[0].penaltyShot;
    mydata.TI_singleShot = d.teams[0].singleShots;
    mydata.coachSequence = d.teams[0].coachSequence;
    /*for (int i=0;i<253;i++){
    mydata.TI_coachMessage[i] = d.teams[0].coachMessage[i];
    }*/
    mydata.TI_COACH_penalty = d.teams[0].coach.penalty;
    mydata.TI_COACH_secsTillUnpenalised = d.teams[0].coach.secsTillUnpenalised;
    mydata.TI_RI_penalty = d.teams[0].players[0].penalty;
    mydata.TI_RI_secsTillUnpenalised = d.teams[0].players[0].secsTillUnpenalised;
    mydata.TI_RI2_penalty = d.teams[0].players[1].penalty;
    mydata.TI_RI2_secsTillUnpenalised = d.teams[0].players[1].secsTillUnpenalised;

    mydata.TI2_teamNUmber = d.teams[1].teamNumber;
    mydata.TI2_teamColour = d.teams[1].teamColour;
    mydata.TI2_score = d.teams[1].score;
    mydata.TI2_penaltyShot = d.teams[1].penaltyShot;
    mydata.TI2_singleShot = d.teams[1].singleShots;
    mydata.coachSequence2 = d.teams[1].coachSequence;
    /*for (int i=0;i<253;i++){
    mydata.TI2_coachMessage[i] = d.teams[1].coachMessage[i];
    }*/
    mydata.TI2_COACH_penalty = d.teams[1].coach.penalty;
    mydata.TI2_COACH_secsTillUnpenalised = d.teams[1].coach.secsTillUnpenalised;
    mydata.TI2_RI_penalty = d.teams[1].players[0].penalty;
    mydata.TI2_RI_secsTillUnpenalised = d.teams[1].players[0].secsTillUnpenalised;
    mydata.TI2_RI2_penalty = d.teams[1].players[1].penalty;
    mydata.TI2_RI2_secsTillUnpenalised = d.teams[1].players[1].secsTillUnpenalised;

    return mydata;
}
