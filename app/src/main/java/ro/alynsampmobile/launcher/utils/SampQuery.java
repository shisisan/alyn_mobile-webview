package ro.alynsampmobile.launcher.utils;

import com.joom.paranoid.Obfuscate;

import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Random;
import java.util.StringTokenizer;

@Obfuscate
public class SampQuery {

    private DatagramSocket socket = null;
    private InetAddress serverip = null;
    private String serveraddress = "";
    private final int serverport;
    private final Random f = new Random();
    private boolean isValidAddr = true;
    private final String charset = "windows-1251";
    private final Random random = new Random();

    public SampQuery(String srvip, int srvport) {
        this.serverport = srvport;
        try {
            serverip = InetAddress.getByName(srvip);
            serveraddress = serverip.getHostAddress();
        } catch (UnknownHostException e) {
            this.isValidAddr = false;
            serveraddress = null;
        }

        try {
            socket = new DatagramSocket();
            socket.setSoTimeout(2000);
        } catch (SocketException e) {
            this.isValidAddr = false;
            serveraddress = null;
        }
    }

    public boolean isOnline() {
        if (isValidAddr && socket != null) {
            byte[] randomBytes = generateRandomBytes();
            byte[] data = null;

            try {
                String pktData = "p" + new String(randomBytes, charset);
                sendPacket(initPacket(pktData));
                data = receiveData();
            } catch (UnsupportedEncodingException e) {
                e.printStackTrace();
            }

            if (data != null && data.length >= 15 &&
                    data[10] == 112 && data[11] == randomBytes[0] &&
                    data[12] == randomBytes[1] && data[13] == randomBytes[2]) {
                return data[14] == randomBytes[3];
            }
        }
        return false;
    }

    private byte[] generateRandomBytes() {
        byte[] randomBytes = new byte[4];
        random.nextBytes(randomBytes);
        randomBytes[0] = (byte) (randomBytes[0] % 100 + 110);
        randomBytes[1] = (byte) (randomBytes[1] % 128);
        randomBytes[2] = (byte) (randomBytes[2] % 128);
        randomBytes[3] = (byte) (randomBytes[3] % 50);
        return randomBytes;
    }

    public String[] getInfo() {
        String[] infos = new String[6];
        try {
            sendPacket(initPacket("i"));
            ByteBuffer buff = ByteBuffer.wrap(receiveData());
            buff.order(ByteOrder.LITTLE_ENDIAN);
            buff.position(11);

            // Password
            infos[0] = String.valueOf(buff.get() == 1 ? 1 : 0);

            // Players
            infos[1] = String.valueOf(buff.getShort());

            // Max Players
            infos[2] = String.valueOf(buff.getShort());

            // Hostname
            int hostnamelength = buff.getInt();
            infos[3] = convert(buff, hostnamelength);

            // Gamemode
            int gamemodelength = buff.getInt();
            infos[4] = convert(buff, gamemodelength);

            // Language
            int languagelength = buff.getInt();
            infos[5] = convert(buff, languagelength);

            return infos;
        } catch (Exception e) {
            return null;
        }
    }

    private DatagramPacket initPacket(String type) {
        DatagramPacket pkt;
        try {
            String pktdata = "SAMP" + "zalupa" + type;

            byte[] IPS;
            IPS = pktdata.getBytes(charset);

            StringTokenizer IP = new StringTokenizer(serveraddress, ".");
            IPS[4] = (byte)Integer.parseInt(IP.nextToken());
            IPS[5] = (byte)Integer.parseInt(IP.nextToken());
            IPS[6] = (byte)Integer.parseInt(IP.nextToken());
            IPS[7] = (byte)Integer.parseInt(IP.nextToken());
            IPS[8] = (byte)(serverport & 255);
            IPS[9] = (byte)(serverport >> 8 & 255);
            pkt = new DatagramPacket(IPS, IPS.length, serverip, serverport);
        } catch (Exception e) {
            pkt = null;
            return pkt;
        }

        return pkt;
    }

    private byte[] receiveData() {
        if (socket == null) {
            return new byte[3072];
        } else {
            byte[] data = new byte[3072];
            DatagramPacket getpacket = null;
            try {
                getpacket = new DatagramPacket(data, 3072);
                socket.receive(getpacket);
            } catch (IOException ignored) {
            }

            return getpacket.getData();
        }
    }

    private void sendPacket(DatagramPacket d) {
        try {
            if (socket != null) {
                socket.send(d);
            }
        } catch (IOException ignored) {
        }

    }

    public void close() {
        if (socket != null) {
            socket.close();
        }
    }

    public long e() {
        long var4;
        long var6;
        try {
            byte[] var1 = this.f();
            StringBuilder var2 = new StringBuilder();
            var2.append("p");
            String var3 = new String(var1, charset);
            var2.append(var3);
            var4 = System.currentTimeMillis();
            sendPacket(initPacket(var2.toString()));
            receiveData();
            var6 = System.currentTimeMillis();
        } catch (UnsupportedEncodingException var8) {
            var8.printStackTrace();
            return -1L;
        }

        return var6 - var4;
    }

    byte[] f() {
        byte[] var1 = new byte[4];
        this.f.nextBytes(var1);
        var1[0] = (byte)(var1[0] % 100 + 110 & 255);
        var1[1] = (byte)(var1[1] % 128);
        var1[2] = (byte)(var1[2] % 128);
        var1[3] = (byte)(var1[3] % 50 & 255);
        return var1;
    }

    private String convert(ByteBuffer buff, int length) throws UnsupportedEncodingException {
        byte[] n = new byte[length];
        try{
            for (int x = 0; x < length; x++)
                n[x] = buff.get();
        }
        catch (Exception ignored) {
        }
        return new String(n,charset);
    }
}