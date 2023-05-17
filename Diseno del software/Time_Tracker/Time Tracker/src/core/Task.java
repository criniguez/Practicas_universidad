package core;

import java.time.Duration;
import java.time.LocalDateTime;
import java.util.ArrayList;
import java.util.List;

/**
 * This class inherits from the abstract class activity,
 * generates tasks that will be in a project, counts the
 * time spent on them to update the project. It implements
 * the composite and is the leaf.
 */
public class Task extends Activity {
  //region __________ATTRIBUTES__________
  private List<Interval> intervalList;
  private boolean isCounting;
  //endregion

  //region __________CONSTRUCTORS__________
  public Task(String name, List<String> tag, LocalDateTime initialDate, LocalDateTime finalDate) {
    super(name, tag, initialDate, finalDate);
    isCounting = false;
  }

  public Task(String name) {
    super(name);
    this.intervalList = new ArrayList<>();
    this.isCounting   = false;
  }
  //endregion

  //region __________GETTERS__________
  public List<Interval> getIntervalList() {
    return this.intervalList;
  }
  //endregion

  //region __________SETTERS__________
  public void setIntervalList(List<Interval> interval) {
    this.intervalList = interval;
  }
  //endregion

  public void start() {
    assert checkInvariant();
    assert !isCounting;
    logger.info("{} starts", name);
    intervalList.add(new Interval(this));

    logger.debug("initial date {}", initialDate);
    initialDate = (initialDate == null) ? intervalList.get(0).getInitialDate() : initialDate;

    logger.debug("parent initial date {}", parent.getInitialDate());
    if (parent.getInitialDate() == null) {
      parent.setInitialDate(initialDate);
    }

    isCounting = true;
    assert checkInvariant();
    assert isCounting;
  }

  public void stop() {
    assert checkInvariant();
    assert isCounting;
    logger.info("{} stops", name);
    intervalList.get(intervalList.size() - 1).finishInterval();
    isCounting = false;
    assert checkInvariant();
    assert !isCounting;
  }

  @Override
  protected boolean checkInvariant() {
    boolean res = super.checkInvariant();
    res &= parent != null;
    res &= intervalList != null;
    return res;
  }

  @Override
  public Duration calculateTotalTime() {
    assert checkInvariant();

    Duration res = Duration.ZERO;

    for (Interval interval : intervalList) {
      res = res.plus(interval.getDuration());
    }

    int val =  res.compareTo(Duration.ZERO);
    boolean cmp = val >= 0;

    assert checkInvariant();
    assert cmp;
    return res;
  }

  @Override
  public void accept(Visitor visitor) {
    assert visitor != null;
    visitor.visitTask(this);
  }

  @Override
  public void update(LocalDateTime time) {
    // In this function, we update all variables that depend on the class ClockTimer. These are the
    // final date we set of the time (we return the class Clock Timer) and the initial date we
    // compare if the initial date are setting are before of the time minus 2 seconds and select
    // this. Finally, update its predecessor.
    assert checkInvariant();

    logger.info("Updating at task {}", name);
    finalDate = time;

    LocalDateTime newDate = time.minusSeconds(2);
    logger.debug("Value {} in initialDate and {} in newDate", initialDate, newDate);
    if (initialDate == null || newDate.isBefore(initialDate)) {
      initialDate = time.minusSeconds(2);
    }

    logger.trace("activity: {} {} {} {}", name, initialDate, finalDate,
        calculateTotalTime().toSeconds());
    parent.update(time);

    assert checkInvariant();
  }

  @Override
  public String toString() {
    return "{'class':'Task'"
        + ",'name':'" + name + "'"
        + ",'tags': " + tagListToString()
        + ",'Initial_date':'" + initialDate + "'"
        + ",'Final_date':'" + finalDate + "'"
        + ",'IntervalList':" + intervalList + "}";
  }
}

