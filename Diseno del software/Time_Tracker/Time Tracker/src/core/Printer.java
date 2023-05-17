package core;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * This class implements the visitor pattern.
 * It prints the entire Composite tree on the
 * screen.
 */
public class Printer implements Visitor {
  protected static final Logger logger = LoggerFactory.getLogger("core");

  private void print(Activity activity, String s) {
    logger.debug("{} {} child of {} initial date: {} final date: {} duration: {} tags: {}",  s,
        activity.getName(), activity.getParentName(), activity.getInitialDate(),
        activity.getFinalDate(), activity.calculateTotalTime().toSeconds(), activity.getTags());
  }

  @Override
  public void visitTask(Task task) {
    print(task, "Task");
    for (Interval interval : task.getIntervalList()) {
      interval.accept(this);
    }
  }

  @Override
  public void visitProject(Project project) {
    print(project, "Project");
    for (Activity child : project.getChildren()) {
      child.accept(this);
    }
  }

  @Override
  public void visitInterval(Interval interval) {
    logger.debug("Interval child of {} initial date: {} final date: {} duration: {}",
        interval.getParentName(),  interval.getInitialDate(), interval.getFinalDate(),
        interval.getDuration().toSeconds());
  }
}
