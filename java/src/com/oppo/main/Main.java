package com.oppo.main;

import java.math.BigInteger;
import java.security.SecureRandom;

/**
 * @author LiHongbing
 * @since 2016年11月4日
 */
public class Main implements Runnable {
    public static final String TAG = Main.class.getSimpleName();

    public Main() {
    }

    public static void main(String[] args) {
        new Main().run();
    }

    @Override
    public void run() {
        SecureRandom random = new SecureRandom();
        String text = new BigInteger(130, random).toString(32);
        System.out.println(text);
    }
}
