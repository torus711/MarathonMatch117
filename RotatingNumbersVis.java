import javax.swing.*;
import java.awt.*;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.awt.geom.AffineTransform;
import java.awt.geom.Rectangle2D;
import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.security.SecureRandom;
import java.util.*;
import java.util.List;


class TestCase {
    final Object worldLock = new Object();
    
    //test parameters
    public static final int minN = 4, maxN = 40;
    public static final int minP = 1, maxP = 10;
    
    //constants
    public static final int minSize = 2;
    public int maxSize = -1;
    public static final int MAX_MOVES = 100000;

    //inputs
    public int N;    
    public int P;    
    public int[][] Grid;
    public long seed;
    
    //outputs
    public int MovePenalty=0;  
    public int LocationPenalty=0;
    public int movesMade=0;
    public Move lastMove;
    
    //other
    public boolean Debug=false;
    public SecureRandom rnd = null;    


    public TestCase(long seed, int n, int p, boolean debug)
    {
      try {
          rnd = SecureRandom.getInstance("SHA1PRNG");
      } catch (Exception e) {
          System.err.println("ERROR: unable to generate test case.");
          System.exit(1);
      }
      
      Debug=debug;

      this.seed=seed;
      rnd.setSeed(seed);    

      //generate random grid size
      if (n==0)
      {
        N = rnd.nextInt(maxN - minN + 1) + minN;    
        if (seed == 1) N = minN;
        if (seed == 2) N = maxN;     
      }
      //use custom grid size
      else
      {
        N = Math.min(Math.max(n, minN), maxN);
      }
      maxSize = N;  

      //generate random weighting parameter
      if (p==0)
      {
        P = rnd.nextInt(maxP - minP + 1) + minP;           
      }
      //use custom weighting parameter
      else
      {
        P = Math.min(Math.max(p, minP), maxP);
      }


      int[] ind=new int[N*N];
      for (int i=0; i<ind.length; i++) ind[i]=i;
      shuffle(ind);
      
      //generate grid
      Grid=new int[N][N];
      for (int i=0; i<N*N; i++)
        Grid[ind[i]/N][ind[i]%N]=(i+1);
                  
      if (Debug) printTest(seed);        
    }
    
    //shuffle the array randomly
    public void shuffle(int[] a)
    {
      for (int i=0; i<a.length; i++)
      {
        int k=(int)(rnd.nextDouble()*(a.length-i)+i);
        int temp=a[i];  
        a[i]=a[k];
        a[k]=temp;  
      }   
    }    
    
    public void printTest(long seed)
    {
      System.out.println("seed = "+seed);
      System.out.println("N = "+N);
      System.out.println("P = "+P);
      System.out.println("Input grid:");
      for (int i=0; i<N; i++)
      {
        for (int k=0; k<N; k++) System.out.print(Grid[i][k]+" ");
        System.out.println();
      }
    }    

    public void makeMove(int r, int c, int size, char dir) throws Exception {
      synchronized (worldLock) {
        lastMove = new Move(r,c,size,dir);
        int[][] temp=new int[size][size];        
        movesMade++;
        MovePenalty+=(int)Math.pow(size-1, 1.5);

        //rotate clockwise
        if (dir=='R')
        {
          for (int r2=0; r2<size; r2++)
            for (int c2=0; c2<size; c2++)
              temp[r2][c2]=Grid[r+size-1-c2][c+r2];                 
        }
        //rotate counter-clockwise
        else
        {
          for (int r2=0; r2<size; r2++)
            for (int c2=0; c2<size; c2++)
              temp[r2][c2]=Grid[r+c2][c+size-1-r2];
        }

        for (int r2=0; r2<size; r2++)
          for (int c2=0; c2<size; c2++)
            Grid[r+r2][c+c2]=temp[r2][c2];           
      }
    }
    
    static class Move
    {
      int r;
      int c;
      int size;
      char dir;
      
      public Move(int R, int C, int S, char D)
      {
        r=R;
        c=C;
        size=S;
        dir=D;
      }
    }
}

class Drawer extends JFrame {
    public static final int EXTRA_WIDTH = 200;
    public static final int EXTRA_HEIGHT = 50;
    public int SZ;

    public TestCase tc;
    public DrawerPanel panel;

    public double animAngle;
    public TestCase.Move animMove;

    public boolean pauseMode = false;

    class DrawerKeyListener extends KeyAdapter {
        public void keyPressed(KeyEvent e) {
            synchronized (keyMutex) {
                if (e.getKeyChar() == ' ') pauseMode = !pauseMode;
                keyPressed = true;
                
                keyMutex.notifyAll();
            }
        }
    }

    int startX = 10;
    int startY = 10;

    public void repaint(int r, int c, int s) {
        panel.repaint(startX + SZ * c - SZ * s / 2, startY + SZ * r - SZ * s / 2, SZ * s * 2, SZ * s * 2);
    }

    class DrawerPanel extends JPanel {
        Font fontGrid;
        Color[] colors;

        public void paintComponent(Graphics g) {
            synchronized (tc.worldLock) {
                // Use antialiasing when painting.
                Graphics2D g2 = (Graphics2D) g;
                g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
                g2.setRenderingHint(RenderingHints.KEY_INTERPOLATION, RenderingHints.VALUE_INTERPOLATION_BILINEAR);
                g2.setRenderingHint(RenderingHints.KEY_RENDERING, RenderingHints.VALUE_RENDER_QUALITY);

                // gray background
                g.setColor(new Color(0xDDDDDD));
                g.fillRect(0, 0, 10000, 10000);
                // white rectangle
                g.setColor(Color.WHITE);
                g.fillRect(startX, startY, tc.N * SZ, tc.N * SZ);

                //paint thin lines between cells
                g.setColor(Color.BLACK);
                for (int i = 0; i <= tc.N; i++)
                    g.drawLine(startX, startY + i * SZ, startX + tc.N * SZ, startY + i * SZ);
                for (int i = 0; i <= tc.N; i++)
                    g.drawLine(startX + i * SZ, startY, startX + i * SZ, startY + tc.N * SZ);

                if (fontGrid == null) {
                    int fontSize = 4;
                    int limit = SZ - 2;
                    if (SZ > 20) limit -= SZ / 10;
                    while (fontSize < 50) {
                        g.setFont(new Font("Arial", Font.PLAIN, fontSize));
                        FontMetrics fm = g2.getFontMetrics();
                        Rectangle2D rc = fm.getStringBounds(String.valueOf(tc.N * tc.N), g2);
                        if (rc.getWidth() >= limit) {
                            fontSize--;
                            break;
                        }
                        fontSize++;
                    }
                    fontGrid = new Font("Arial", Font.PLAIN, fontSize);
                }
                g.setFont(fontGrid);
                FontMetrics fm = g2.getFontMetrics();

                AffineTransform ct = null;
                AffineTransform rt = null;
                if (animMove != null) {
                    ct = g2.getTransform();
                    rt = new AffineTransform();
                    rt.rotate(animAngle, startX + animMove.c * SZ + SZ * animMove.size / 2.0, startY + animMove.r * SZ + SZ * animMove.size / 2.0);
                }

                if (colors == null) {
                    colors = new Color[2 * tc.N - 1];
                    for (int i = 0; i < colors.length; i++) {
                        int alpha = 255 - (int) Math.round(255 * (1 - Math.min(1, i * 3.0 / (2 * (tc.N - 1)))));
                        colors[i] = new Color(alpha, 255, alpha);
                    }
                }

                Rectangle clip = g.getClipBounds();
                for (int step = 0; step <= (animMove != null ? 1 : 0); step++) {
                    if (step == 1) g2.transform(rt);
                    for (int r = 0; r < tc.N; r++) {
                        if (startY + r * SZ > clip.getMaxY()) break;
                        if (startY + r * SZ + SZ < clip.getMinY()) continue;
                        if (step == 1 && (r < animMove.r || r >= animMove.r + animMove.size)) continue;
                        for (int c = 0; c < tc.N; c++) {
                            if (startX + c * SZ > clip.getMaxX()) break;
                            if (startX + c * SZ + SZ < clip.getMinX()) continue;
                            if (step == 1 && (c < animMove.c || c >= animMove.c + animMove.size)) continue;
                            //paint squares green based on how close they are to the target location
                            int requiredR = (tc.Grid[r][c] - 1) / tc.N;
                            int requiredC = (tc.Grid[r][c] - 1) % tc.N;
                            int dist = Math.abs(r - requiredR) + Math.abs(c - requiredC);
                            boolean in = animMove != null && r >= animMove.r && r < animMove.r + animMove.size && c >= animMove.c && c < animMove.c + animMove.size;

                            g.setColor(in && step == 0 ? Color.gray : colors[dist]);
                            g.fillRect(startX + SZ * c + 1, startY + SZ * r + 1, SZ - 1, SZ - 1);

                            //draw numbers
                            if (in ^ (step == 0)) {
                                g.setColor(Color.BLACK);
                                String str = String.valueOf(tc.Grid[r][c]);
                                Rectangle2D rc = fm.getStringBounds(str, g2);
                                int x = (SZ - (int) rc.getWidth()) / 2;
                                int y = (SZ - (int) rc.getHeight()) / 2 + fm.getAscent();
                                if (step == 1) {
                                    AffineTransform st = new AffineTransform();
                                    st.rotate(-animAngle, startX + c * SZ + SZ / 2.0, startY + r * SZ + SZ / 2.0);
                                    g2.transform(st);
                                }
                                g.drawString(str, startX + SZ * c + x, startY + SZ * r + y);
                                if (step == 1) {
                                    AffineTransform st = new AffineTransform();
                                    st.rotate(animAngle, startX + c * SZ + SZ / 2.0, startY + r * SZ + SZ / 2.0);
                                    g2.transform(st);
                                    g.drawRect(startX + SZ * c, startY + SZ * r, SZ, SZ);
                                }
                            }
                        }
                    }
                    if (step == 1) g2.setTransform(ct);
                }

                TestCase.Move move = animMove != null ? animMove : tc.lastMove;
                if (move != null) {
                    if (rt != null) g2.transform(rt);
                    // draw last move
                    Stroke oldStroke = g2.getStroke();
                    g2.setStroke(new BasicStroke(3));

                    if (move.dir == 'R') g.setColor(Color.BLUE);
                    else g.setColor(Color.RED);
                    g.drawRect(startX + move.c * SZ, startY + move.r * SZ, SZ * move.size, SZ * move.size);

                    g2.setStroke(oldStroke);
                    if (ct != null) g2.setTransform(ct);
                }
                
              //display score and info
              g.setColor(Color.BLACK);                    
              g.setFont(new Font("Arial",Font.BOLD,16));
              g.drawString("N: "+tc.N,SZ*tc.N+25,50);   
              g.drawString("P: "+tc.P,SZ*tc.N+25,80);   
              g.drawString("Moves: "+tc.movesMade,SZ*tc.N+25,110);   
              g.drawString("Move Penalty: "+tc.MovePenalty,SZ*tc.N+25,140);   
              g.drawString("Loc Penalty: "+tc.LocationPenalty,SZ*tc.N+25,170);   
              g.drawString("SCORE: "+(tc.MovePenalty+tc.LocationPenalty),SZ*tc.N+25,200);   
            }
        }
    }

    class DrawerWindowListener extends WindowAdapter {
        public void windowClosing(WindowEvent event) {
            RotatingNumbersVis.stopSolution();
            System.exit(0);
        }
    }

    final Object keyMutex = new Object();
    boolean keyPressed;

    public void processPause() {
        synchronized (keyMutex) {
            if (!pauseMode) {
                return;
            }
            keyPressed = false;
            while (!keyPressed) {
                try {
                    keyMutex.wait();
                } catch (InterruptedException e) {
                    // do nothing
                }
            }
        }
    }

    public Drawer(TestCase tc_, int SZ) {
        super();

        this.tc = tc_;

        panel = new DrawerPanel();
        getContentPane().add(panel);
        addWindowListener(new DrawerWindowListener());
        addKeyListener(new DrawerKeyListener());


        if (SZ==0)
        {
          Insets frameInsets = getInsets();
          int fw = frameInsets.left + frameInsets.right;
          int fh = frameInsets.top + frameInsets.bottom;
          Toolkit toolkit = Toolkit.getDefaultToolkit();
          Dimension screenSize = toolkit.getScreenSize();
          Insets screenInsets = toolkit.getScreenInsets(getGraphicsConfiguration());
          screenSize.width -= screenInsets.left + screenInsets.right;
          screenSize.height -= screenInsets.top + screenInsets.bottom;
          SZ = Math.min((screenSize.width - fw - EXTRA_WIDTH) / tc.N, (screenSize.height - fh - EXTRA_HEIGHT) / tc.N);
          setSize(tc.N*SZ + EXTRA_WIDTH + fw, Math.max(420, tc.N*SZ + EXTRA_HEIGHT + fh));  
        }
        else
        {        
          setSize(tc.N*SZ + EXTRA_WIDTH, tc.N*SZ + EXTRA_HEIGHT);  
        } 

        this.SZ=SZ;

        setTitle("Visualizer for RotatingNumbers, seed = "+tc.seed);
        setResizable(false);
        setVisible(true);

       
    }
}


public class RotatingNumbersVis {
    public static String execCommand = null;
    public static long seed = 1;
    public static boolean vis = true;
    public static int SZ = 20;
    public static int N = 0;
    public static int P = 0;
    public static int delay = 500;
    public static boolean startPaused = false;
    public static boolean debug = false;

    public static Process solution;

    public double runTest() {
        solution = null;

        try {
            solution = Runtime.getRuntime().exec(execCommand);
        } catch (Exception e) {
            System.err.println("ERROR: Unable to execute your solution using the provided command: "
                    + execCommand + ".");
            return -1;
        }

        BufferedReader reader = new BufferedReader(new InputStreamReader(solution.getInputStream()));
        PrintWriter writer = new PrintWriter(solution.getOutputStream());
        output = new ErrorReader(solution.getErrorStream());
        output.start();

        TestCase tc = new TestCase(seed,N,P,debug);
       
        writer.println(tc.N);
        writer.println(tc.P);
        for (int i=0; i<tc.N; i++)
          for (int k=0; k<tc.N; k++)
            writer.println(tc.Grid[i][k]);
        writer.flush();    
        
                
        Drawer drawer = null;
        if (vis) {
            drawer = new Drawer(tc, SZ);
            if (startPaused) drawer.pauseMode = true;
        }       
        
        int numMoves;
        String[] allMoves;
        
        
        try {
            numMoves = Integer.parseInt(reader.readLine());
        } catch (Exception e) {
            System.err.println("ERROR: Invalid number of moves detected");
            return -1;
        }
        if (numMoves < 0)
        {
          System.err.println("ERROR: Invalid number of moves detected");
          return -1;          
        }
        if (numMoves > tc.MAX_MOVES)
        {
          System.err.println("ERROR: Return array is too large.");
          return -1;
        }        

        allMoves = new String[numMoves];
        try {
          for (int i = 0; i < numMoves; i++)
            allMoves[i] = reader.readLine();
        } catch (Exception e) {
            System.err.println("ERROR: Unable to read output.");
            return -1;
        }

        //recompute location penalty
        tc.LocationPenalty=0;
        for (int r2=0; r2<tc.N; r2++)
          for (int c2=0; c2<tc.N; c2++)
          {
            int requiredR=(tc.Grid[r2][c2]-1)/tc.N;
            int requiredC=(tc.Grid[r2][c2]-1)%tc.N;
            tc.LocationPenalty+=tc.P*(Math.abs(r2-requiredR)+Math.abs(c2-requiredC));
          }         
        
        
        try {
          for (int i=0; i<numMoves; i++) {
              String smove = allMoves[i];
              if (debug) System.err.println("executing "+smove);
              String[] s = smove.split(" ");
              if (s.length!=4) {
                  System.err.println("ERROR: The move command with index "+i+" does not contain 4 space seperated values. Value is ["+smove+"]");
                  return -1;
              }
              int r = -1;
              int c = -1;
              try
              {
                r = Integer.parseInt(s[0]);
                c = Integer.parseInt(s[1]);
              } catch (Exception e) {
                System.err.println("ERROR: invalid coordinates "+s[0]+" "+s[1]);
                return -1;
              }
              if (r<0 || r>=tc.N || c<0 || c>=tc.N) {
                  System.err.println("ERROR: (" + r + "," + c + ") outside of bounds.");
                  return -1;
              }
              int size = -1;
              try
              {
                size = Integer.parseInt(s[2]);
              } catch (Exception e) {
                System.err.println("ERROR: invalid size "+s[2]);
                return -1;
              }
              if (size<tc.minSize || size>tc.maxSize)
              {
                System.err.println("ERROR: size must be between "+tc.minSize+" and "+tc.maxSize+", but "+size+" was given");
                return -1;                
              }
              if (r+size>tc.N || c+size>tc.N)
              {
                  System.err.println("ERROR: move "+smove+" is out of bounds.");
                  return -1;
              }
              s[3]=s[3].trim();
              char dir = s[3].charAt(0);
              if (s[3].length()!=1 || !(dir=='R' || dir=='L'))
              {
                  System.err.println("ERROR: direction "+s[3]+" is illegal.");
                  return -1;
              }

              //Animate
              if (vis && delay >= 20) {
                  int rotDelay = delay / 100;
                  if (rotDelay < 10) rotDelay = 10;
                  int steps = delay / rotDelay;
                  double stepAngle = Math.PI / 2 / steps;
                  if (dir == 'L') stepAngle *= -1;
                  synchronized (tc.worldLock) {
                      drawer.animMove = new TestCase.Move(r, c, size, dir);
                  }
                  drawer.repaint();
                  for (int j = 1; j <= steps; j++) {
                      synchronized (tc.worldLock) {
                          drawer.animAngle = stepAngle * j;
                      }
                      drawer.repaint(r, c, size);
                      try {
                          Thread.sleep(rotDelay);
                      } catch (Exception e) {
                      }
                  }
                  synchronized (tc.worldLock) {
                      drawer.animMove = null;
                      drawer.animAngle = 0;
                  }
              }

              // perform the move
              tc.makeMove(r,c,size,dir);

              //recompute location penalty
              tc.LocationPenalty=0;
              for (int r2=0; r2<tc.N; r2++)
                for (int c2=0; c2<tc.N; c2++)
                {
                  int requiredR=(tc.Grid[r2][c2]-1)/tc.N;
                  int requiredC=(tc.Grid[r2][c2]-1)%tc.N;
                  tc.LocationPenalty+=tc.P*(Math.abs(r2-requiredR)+Math.abs(c2-requiredC));
                }              
              
              if (vis && delay > 0 && i < numMoves - 1) {
                  drawer.repaint();
                  drawer.processPause();
                  try {
                      Thread.sleep(delay);
                  } catch (Exception e) {
                      // do nothing
                  }
              }
            }
            if (vis) {
                drawer.repaint();
            }
        } catch (Exception e) {
            System.err.println("ERROR: Unable to process the move commands from your solution.");
            return -1;
        }

        stopSolution();
        
        return tc.MovePenalty+tc.LocationPenalty;
    }

    public static void stopSolution() {
        if (solution != null) {
            try {
                solution.destroy();
            } catch (Exception e) {
                // do nothing
            }
        }
    }

    public static void main(String[] args) {
        for (int i = 0; i < args.length; i++)
            if (args[i].equals("-exec")) {
                execCommand = args[++i];
            } else if (args[i].equals("-seed")) {
                seed = Long.parseLong(args[++i]);
            } else if (args[i].equals("-novis")) {
                vis = false;
            } else if (args[i].equals("-debug")) {
                debug = true;                
            } else if (args[i].equals("-sz")) {
                SZ = Integer.parseInt(args[++i]);
            } else if (args[i].equals("-N")) {
                N = Integer.parseInt(args[++i]);                
            } else if (args[i].equals("-P")) {
                P = Integer.parseInt(args[++i]);                    
            } else if (args[i].equals("-delay")) {
                delay = Integer.parseInt(args[++i]);
            } else if (args[i].equals("-pause")) {
                startPaused = true;
            } else {
                System.out.println("WARNING: unknown argument " + args[i] + ".");
            }            

        if (execCommand == null) {
            System.err.println("ERROR: You did not provide the command to execute your solution." +
                    " Please use -exec <command> for this.");
            System.exit(1);
        }

        double score=-1;
        RotatingNumbersVis vis = new RotatingNumbersVis();
        try {
            score=vis.runTest();
            System.out.println("Score = " + score);
        } catch (RuntimeException e) {
            System.err.println("ERROR: Unexpected error while running your test case.");
            e.printStackTrace();
            RotatingNumbersVis.stopSolution();
        }
    }
    
    public static ErrorReader output;    
}

class ErrorReader extends Thread{
    InputStream error;
    StringBuilder sb = new StringBuilder();
    public ErrorReader(InputStream is) {
        error = is;
    }
    public void run() {
        try {
            byte[] ch = new byte[50000];
            int read;
            while ((read = error.read(ch)) > 0)
            {   String s = new String(ch,0,read);
                System.out.print(s);
                sb.append(s);
                System.out.flush();
            }
        } catch(Exception e) { }
    }
}
