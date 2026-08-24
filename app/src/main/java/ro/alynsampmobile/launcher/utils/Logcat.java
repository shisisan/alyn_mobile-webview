package ro.alynsampmobile.launcher.utils;

import android.content.Context;

import com.joom.paranoid.Obfuscate;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.List;

import kotlin.collections.CollectionsKt;
import kotlin.io.FilesKt;
import kotlin.jvm.internal.Intrinsics;
import kotlin.text.Charsets;
import kotlin.text.StringsKt;

@Obfuscate
public final class Logcat {
    private static String PATH_LOGCAT;
    private LogDumper mLogDumper;
    private final int mPId;

    public Logcat(Context context) {
        init(context);
        this.mPId = android.os.Process.myPid();
    }

    public void init(Context context) {
        Intrinsics.checkNotNullParameter(context, "context");
        String str = null;
        File externalFilesDir = context.getExternalFilesDir(null);
        if (externalFilesDir != null) {
            str = externalFilesDir.getAbsolutePath();
        }
        PATH_LOGCAT = str + "/logcat";
        String str2 = PATH_LOGCAT;
        Intrinsics.checkNotNull(str2);
        File file = new File(str2);
        if (!file.exists()) {
            file.mkdirs();
        }
    }

    public void start() {
        if (this.mLogDumper == null) {
            this.mLogDumper = new LogDumper(String.valueOf(this.mPId), PATH_LOGCAT);
        }
        try {
            LogDumper logDumper = this.mLogDumper;
            if (logDumper != null) {
                logDumper.start();
            }
        } catch (IllegalThreadStateException e) {
            e.printStackTrace();
        }
    }

    public void stop() {
        LogDumper logDumper = this.mLogDumper;
        if (logDumper != null) {
            if (logDumper != null) {
                logDumper.stopLogs();
            }
            this.mLogDumper = null;
        }
    }

    private final class LogDumper extends Thread {
        private String cmds;
        private Process logcatProc;
        private final String mPID;
        private BufferedReader mReader;
        private boolean mRunning;
        private FileOutputStream out;

        public LogDumper(String str, String str2) {
            Intrinsics.checkNotNullParameter(str, "mPID");
            this.mPID = str;
            File file = new File(str2, "Alyn_SAMPMOBILE.log");
            if (file.exists()) {
                if (file.length() > 33554432) {
                    file.delete();
                } else if (file.length() > 16777216) {
                    try {
                        List<String> readLinesdefault = FilesKt.readLines(file, Charsets.UTF_8);
                        List<String> subList = readLinesdefault.subList(readLinesdefault.size() / 2, readLinesdefault.size());
                        String joinedText = CollectionsKt.joinToString(subList, "\n", "", "", -1, "...", null);
                        FilesKt.writeText(file, joinedText, Charsets.UTF_8);

                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }
            }
            try {
                this.out = new FileOutputStream(file, true);
            } catch (FileNotFoundException e2) {
                e2.printStackTrace();
            }
            String str3 = this.mPID;
            this.cmds = "logcat | grep \"(" + str3 + ")\"";
        }

        public String getCmds() {
            return this.cmds;
        }

        public void setCmds(String str) {
            this.cmds = str;
        }

        public void start() {
            super.start();
            this.mRunning = true;
        }

        public void stopLogs() {
            this.mRunning = false;
        }

        public boolean isRunning() {
            return this.mRunning;
        }

        public void run() {
            try {
                this.logcatProc = Runtime.getRuntime().exec(this.cmds);
                Process process = this.logcatProc;
                Intrinsics.checkNotNull(process);
                this.mReader = new BufferedReader(new InputStreamReader(process.getInputStream()), 1024);
                while (true) {
                    if (!this.mRunning) {
                        break;
                    }
                    BufferedReader bufferedReader = this.mReader;
                    Intrinsics.checkNotNull(bufferedReader);
                    String readLine = bufferedReader.readLine();
                    if (readLine == null) {
                        break;
                    } else if (!this.mRunning) {
                        break;
                    } else {
                        Intrinsics.checkNotNull(readLine);
                        if (readLine.length() != 0) {
                            if (!StringsKt.contains(readLine, "AudioTrack", false)) {
                                if (this.out != null && StringsKt.contains(readLine, this.mPID, false)) {
                                    FileOutputStream fileOutputStream = this.out;
                                    Intrinsics.checkNotNull(fileOutputStream);
                                    byte[] bytes = (readLine + "\n").getBytes(Charsets.UTF_8);
                                    Intrinsics.checkNotNullExpressionValue(bytes, "getBytes(...)");
                                    fileOutputStream.write(bytes);
                                }
                            }
                        }
                    }
                }
                Process process2 = this.logcatProc;
                if (process2 != null) {
                    Intrinsics.checkNotNull(process2);
                    process2.destroy();
                    this.logcatProc = null;
                }
                BufferedReader bufferedReader2 = this.mReader;
                if (bufferedReader2 != null) {
                    try {
                        Intrinsics.checkNotNull(bufferedReader2);
                        bufferedReader2.close();
                        this.mReader = null;
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
                FileOutputStream fileOutputStream2 = this.out;
                if (fileOutputStream2 != null) {
                    try {
                        Intrinsics.checkNotNull(fileOutputStream2);
                        fileOutputStream2.close();
                    } catch (IOException e2) {
                        e2.printStackTrace();
                    }
                    this.out = null;
                }
            } catch (IOException e3) {
                e3.printStackTrace();
                Process process3 = this.logcatProc;
                if (process3 != null) {
                    Intrinsics.checkNotNull(process3);
                    process3.destroy();
                    this.logcatProc = null;
                }
                BufferedReader bufferedReader3 = this.mReader;
                if (bufferedReader3 != null) {
                    try {
                        Intrinsics.checkNotNull(bufferedReader3);
                        bufferedReader3.close();
                        this.mReader = null;
                    } catch (IOException e4) {
                        e4.printStackTrace();
                    }
                }
                FileOutputStream fileOutputStream3 = this.out;
                if (fileOutputStream3 != null) {
                    try {
                        Intrinsics.checkNotNull(fileOutputStream3);
                        fileOutputStream3.close();
                    } catch (IOException e5) {
                        e5.printStackTrace();
                    }
                }
            } catch (Throwable th) {
                Process process4 = this.logcatProc;
                if (process4 != null) {
                    Intrinsics.checkNotNull(process4);
                    process4.destroy();
                    this.logcatProc = null;
                }
                BufferedReader bufferedReader4 = this.mReader;
                if (bufferedReader4 != null) {
                    try {
                        Intrinsics.checkNotNull(bufferedReader4);
                        bufferedReader4.close();
                        this.mReader = null;
                    } catch (IOException e6) {
                        e6.printStackTrace();
                    }
                }
                FileOutputStream fileOutputStream4 = this.out;
                if (fileOutputStream4 != null) {
                    try {
                        Intrinsics.checkNotNull(fileOutputStream4);
                        fileOutputStream4.close();
                    } catch (IOException e7) {
                        e7.printStackTrace();
                    }
                    this.out = null;
                }
                throw th;
            }
        }
    }
}
