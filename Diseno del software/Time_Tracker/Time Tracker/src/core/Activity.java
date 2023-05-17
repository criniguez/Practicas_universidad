package core;

import java.time.Duration;
import java.time.LocalDateTime;
import java.util.ArrayList;
import java.util.List;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * This class is the abstract class from which
 * task and project inherit.  It implements the
 * functions that are common to both classes, no
 * object of this class is instantiated, implements
 * the composite and is the component.
 */
public abstract class Activity {
  //region __________ATTRIBUTES__________
  protected Project parent;
  protected final String name;
  protected final List<String> tags;
  protected LocalDateTime finalDate;
  protected LocalDateTime initialDate;
  protected static final Logger logger = LoggerFactory.getLogger("core");
  //endregion

  //region __________CONSTRUCTORS__________
  protected Activity(String name, List<String> tags,
                     LocalDateTime initialDate, LocalDateTime finalDate) {
    this.tags = tags;
    this.name = name;
    this.finalDate = finalDate;
    this.initialDate = initialDate;
  }

  protected Activity(String name) {
    this.name = name;
    this.parent = null;
    this.tags = new ArrayList<>();
  }
  //endregion

  //region __________GETTERS__________
  public List<String> getTags() {
    return tags;
  }

  public String getName() {
    return name;
  }

  public Project getParent() {
    return parent;
  }

  public String getParentName() {
    return (parent == null) ? "null" : parent.getName();
  }

  public LocalDateTime getInitialDate() {
    return initialDate;
  }

  public LocalDateTime getFinalDate() {
    return finalDate;
  }
  //endregion

  //region __________SETTERS__________
  public void setParent(Project parent) {
    this.parent = parent;
  }

  public void setInitialDate(LocalDateTime time) {
    initialDate = time;
  }
  //endregion

  public void addTag(String tag) {
    tags.add(tag);
  }

  protected  String tagListToString() {
    String stag = "[";
    int size = this.tags.size();


    if (size >= 1) {
      for (int i = 0; i < size - 1; i++) {
        stag += "'" + this.tags.get(i) + "', ";
      }
      stag += "'" + this.tags.get(size - 1) + "'";
    } else {
      logger.warn("Project does not have Tags");
    }
    stag += "]";
    logger.debug(stag);

    return stag;
  }

  protected boolean checkInvariant() {
    boolean res = name != null;
    res &= tags != null;

    if (initialDate != null && finalDate != null) {
      boolean bool = initialDate.equals(finalDate);
      bool |= initialDate.isBefore(finalDate);
      res &= bool;
    }

    return res;
  }

  public abstract Duration calculateTotalTime();

  public abstract void update(LocalDateTime time);

  public abstract void accept(Visitor visitor);
}