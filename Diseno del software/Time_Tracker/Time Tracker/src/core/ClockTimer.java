package core;

import java.time.LocalDateTime;
import java.util.Observable;
import java.util.Timer;
import java.util.TimerTask;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;



/**
 * This class Generates a clock that will notify that time
 * changes every 2 seconds. The class implements Singleton
 * just to be instantiated once. Furthermore, implements
 * the Observer pattern.
 */
public class ClockTimer extends Observable {
  //region __________ATTRIBUTES__________
  private static ClockTimer instance;
  private LocalDateTime time;
  private static final Logger logger = LoggerFactory.getLogger("core");
  //endregion

  //region __________CONSTRUCTOR__________
  private ClockTimer() {
    Timer t = new Timer();
    logger.info("A clock has been instantiated");
    TimerTask tt = new TimerTask() {
      @Override
      public void run() {
        time = LocalDateTime.now();
        setChanged();
        notifyObservers();
        logger.info("Clock notified time");
      }
    };
    t.scheduleAtFixedRate(tt, 0, 2000);
  }
  //endregion

  //region ____________GETTERS____________
  public static ClockTimer getInstance() {
    if (instance == null) {
      instance = new ClockTimer();
    }
    return instance;
  }

  public LocalDateTime getTime() {
    return time;
  }
  //endregion
}

