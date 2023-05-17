package core;

import java.time.Duration;
import java.time.LocalDateTime;
import java.util.Observable;
import java.util.Observer;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * This class generates a time interval dedicated to a task
 * and is responsible for updating it. It implements the
 * Observer pattern.
 */
public class Interval implements Observer {
  //region ____________ATTRIBUTES____________
  private Task parent;
  private LocalDateTime initialDate;
  private LocalDateTime finalDate;
  private ClockTimer clock;
  private Duration duration;
  private static final Logger logger = LoggerFactory.getLogger("core");
  //endregion

  //region ____________CONSTRUCTORS____________
  public Interval(LocalDateTime initialDate, LocalDateTime finalDate, Duration duration) {
    this.finalDate = finalDate;
    this.initialDate = initialDate;
    this.duration = duration;
  }

  public Interval(Task parent) {
    LocalDateTime aux = LocalDateTime.now();
    finalDate = aux;
    initialDate = aux;
    this.parent = parent;
    this.duration = Duration.ZERO;
    this.clock = ClockTimer.getInstance();
    this.clock.addObserver(this);
  }
  //endregion

  //region ____________GETTERS____________
  public Duration getDuration() {
    return duration;
  }

  public LocalDateTime getInitialDate() {
    return initialDate;
  }

  public LocalDateTime getFinalDate() {
    return finalDate;
  }

  public String getParentName() {
    return this.parent.getName();
  }
  //endregion

  //region ____________SETTERS____________
  public void setParent(Task parent) {
    this.parent = parent;
  }
  //endregion

  public void finishInterval() {
    logger.info("Entering finishInterval");
    clock.deleteObserver(this);
  }

  public void accept(Printer visitor) {
    visitor.visitInterval(this);
  }

  @Override
  public void update(Observable observable, Object arg) {
    // In this function, we update all variables that depend on the class ClockTimer. These are the
    // final date we set of the time (we return the class Clock Timer), the duration that we
    // increment of 2 seconds and the initial date we compare if the initial date are setting are
    // before of the time minus 2 seconds and select this. Finally, update its predecessor.
    logger.info("Updating at interval");

    LocalDateTime time = clock.getTime();
    finalDate = time;
    duration = duration.plusSeconds(2);

    LocalDateTime newDate = time.minusSeconds(2);
    logger.debug("Value {} in initialDate and {} in newDate", initialDate, newDate);
    if (initialDate == null || newDate.isBefore(initialDate)) {
      initialDate = time.minusSeconds(2);
    }

    logger.trace("{} {} {}", initialDate, finalDate, duration.toSeconds());
    parent.update(time);
  }

  public String toString() {
    return "{"
        + "'Initial_date':'" + initialDate + "'"
        + ",'Final_date':'" + finalDate + "'"
        + ",'duration':'" + duration + "'}";
  }
}
