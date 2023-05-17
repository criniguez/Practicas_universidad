package utils;

import core.Activity;
import core.Interval;
import core.Project;
import core.Task;
import core.Visitor;
import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * This class implements the visitor pattern. It
 * generates an object that will search, through
 * the composite, for activities containing the tags
 * selected by a user.
 */
public class SearchByTag implements Visitor {
  //region __________ATTRIBUTES__________
  private final List<String> tag;
  private final Map<String, List<String>> mapActivities;
  protected static final Logger logger = LoggerFactory.getLogger("utils");
  //endregion

  //region __________CONSTRUCTORS__________
  public SearchByTag(List<String> tags) {
    this.tag = new ArrayList<>();
    mapActivities = new LinkedHashMap<>();

    for (String tag : tags) {
      this.tag.add(tag);
      mapActivities.put(tag, new ArrayList<>());
    }
  }
  //endregion

  //region __________GETTERS__________
  public Map<String, List<String>> getMapActivities() {
    return mapActivities;
  }
  //endregion

  private void searcher(Activity activity) {
    logger.info("Entering searcher");

    for (String tag : activity.getTags()) {
      String lowerTag = tag.toLowerCase();
      for (String stag : this.tag) {
        String lowerStag = stag.toLowerCase();
        if (lowerStag.equals(lowerTag)) {
          logger.debug("They are the same {} {}", tag, stag);
          mapActivities.get(stag).add(activity.getName());
        }
      }
    }
    logger.trace(mapActivities.toString());
  }

  @Override
  public void visitTask(Task task) {
    logger.info("Entering Visit Task");
    searcher(task);
  }

  @Override
  public void visitProject(Project project) {
    logger.info("Entering project Task");
    searcher(project);
    for (Activity child : project.getChildren()) {
      child.accept(this);
    }
  }

  @Override
  public void visitInterval(Interval interval) {
    //Does not do anything
    logger.warn("Should not reach here");
  }
}
