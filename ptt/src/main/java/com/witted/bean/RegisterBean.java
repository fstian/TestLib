package com.witted.bean;

public class RegisterBean {




    private String bedName;

    public String getBedName() {
        return bedName;
    }

    public void setBedName(String bedName) {
        this.bedName = bedName;
    }

    @Override
    public String toString() {
        return "RegisterBean{" +
                "bedName='" + bedName + '\'' +
                '}';
    }
}
