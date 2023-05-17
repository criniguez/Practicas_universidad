package core;

import java.time.Duration;
import java.time.LocalDateTime;
import java.util.ArrayList;
import java.util.List;

/**
 * This class inherits from the abstract class
 * activity, it generates the projects that
 * count the time spent on themselves or update
 * other projects where they are. It implements
 * the composite, it will consist of projects
 * and tasks.
 */
public class Project extends Activity {
  //region __________ATTRIBUTES__________
  private List<Activity> children;
  //endregion

  //region __________CONSTRUCTORS__________
  public Project(String name, List<String> tag,
                 LocalDateTime initialDate, LocalDateTime finalDate) {
    super(name, tag, initialDate, finalDate);
  }

  public Project(String name) {
    super(name);
    this.children = new ArrayList<>();
  }
  //endregion

  //region __________GETTERS__________
  public List<Activity> getChildren() {
    return children;
  }
  //endregion

  //region __________SETTERS__________
  public void setChildren(List<Activity> children) {
    this.children = children;
  }

  //endregion
  @Override
  public void accept(Visitor visitor) {
    assert visitor != null;
    visitor.visitProject(this);
  }

  public void removeChild(Activity activity) {
    assert checkInvariant();
    assert activity != null;
    int size = children.size();
    children.remove(activity);
    assert size > children.size();
    assert checkInvariant();
  }

  public void addChild(Activity activity) {
    assert checkInvariant();
    assert activity != null;
    assert children != null;

    logger.debug("Adding child: {}", activity);

    activity.setParent(this);
    children.add(activity);

    int size = children.size();

    assert activity.getParent() != null;
    assert size < children.size();
    assert checkInvariant();
  }

  @Override
  public Duration calculateTotalTime() {
    assert checkInvariant();

    Duration res = Duration.ZERO;

    for (Activity child : children) {
      res = res.plus(child.calculateTotalTime());
    }

    int val =  res.compareTo(Duration.ZERO);
    boolean cmp = val >= 0;

    assert checkInvariant();
    assert cmp;
    return res;
  }

  @Override
  public void update(LocalDateTime time) {
    // In this function, we update all variables that depend on the class ClockTimer. These are the
    // final date we set of the time (we return the class Clock Timer) and the initial date we
    // compare if the initial date are setting are before of the time minus 2 seconds and select
    // this. Finally, update its predecessor.
    assert checkInvariant();

    logger.info("Updating at project {}", name);
    LocalDateTime newDate = time.minusSeconds(2);
    finalDate = time;

    logger.debug("Value {} in initialDate and {} in newDate", initialDate, newDate);
    if (initialDate == null || newDate.isBefore(initialDate)) {
      initialDate = time.minusSeconds(2);
    }

    logger.trace("activity: {} {} {} {}", name, initialDate, finalDate,
        calculateTotalTime().toSeconds());

    logger.debug("parent is {}", parent);
    if (parent != null) {
      parent.update(time);
    } else {
      logger.warn("{} is null", this.getName());
    }

    assert checkInvariant();
  }

  @Override
  public String toString() {
    return "{'class':'Project'"
        + ",'name':'" + name + "'"
        + ",'tags': " + tagListToString()
        + ",'Initial_date':'" + initialDate + "'"
        + ",'Final_date':'" + finalDate + "'"
        + ",'children':" + children + "}";

  }

}
