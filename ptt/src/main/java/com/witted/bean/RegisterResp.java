package com.witted.bean;

public class RegisterResp {
    public int status;


    public String text;

    public RegisterBean result;



    public RegisterResp() {
    }

    public RegisterResp(int status, String text) {
        this.status = status;
        this.text = text;

    }


    public RegisterResp(int status, String text, RegisterBean result) {
        this.status = status;
        this.text = text;
        this.result = result;
    }

    public int getStatus() {
        return status;
    }

    public void setStatus(int status) {
        this.status = status;
    }

    public RegisterBean getResult() {
        return result;
    }

    public void setResult(RegisterBean registerBean) {
        this.result = registerBean;
    }


    public static RegisterResp registerOK(RegisterBean registerBean){

        return new RegisterResp(200,"register ok",registerBean);
    }

    public static RegisterResp heartBeatBack(){
        return new RegisterResp(200,"heart back");
    }

    public static RegisterResp registerFail(String err){
        return new RegisterResp(400,err);

    }



}
