package core;

import java.io.FileWriter;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.time.Duration;
import java.time.LocalDateTime;
import java.util.ArrayList;
import java.util.List;
import org.json.JSONArray;
import org.json.JSONObject;
import org.json.JSONTokener;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;


/**
 *  This class generates a file that saves the whole
 *  tree created with the composite or if the file is
 *  already created, it updates this file. In addition,
 *  it creates a composite tree from a file where all
 *  its variables are defined.
 */
public class Json {
  protected static final Logger logger = LoggerFactory.getLogger("core");

  public void save(Project root, String nameFile) {
    // This function saves the tree of composite in a JSON file
    try {
      JSONObject json = new JSONObject(root.toString());
      FileWriter myWriter = new FileWriter(nameFile);
      myWriter.write(json.toString());
      myWriter.close();
      logger.info("Successfully wrote to the file.");
    } catch (IOException e) {
      logger.error("An error occurred.");
      e.printStackTrace();
    }
  }

  private Interval makeInterval(JSONObject json) {
    LocalDateTime initialDate = LocalDateTime.parse(json.getString("Initial_date"));
    LocalDateTime finalDate = LocalDateTime.parse(json.getString("Final_date"));
    Duration duration = Duration.parse(json.getString("duration"));
    return new Interval(initialDate, finalDate, duration);
  }

  private Task makeTask(JSONObject json) {
    // Parse task json format to generate a task object

    List<Interval> intervalList = new ArrayList<>();
    String name = json.getString("name");
    JSONArray tagList = json.getJSONArray("tags");
    JSONArray intervals = json.getJSONArray("IntervalList");

    List<String> tags = new ArrayList<>();
    for (int i = 0; i < tagList.length(); i++) {
      tags.add(tagList.getString(i));
    }

    Task res;
    String stringDate = json.getString("Initial_date");
    if (stringDate.equals("null")) {
      res = new Task(name, tags, null, null);
    } else {
      LocalDateTime initialDate = LocalDateTime.parse(stringDate);
      LocalDateTime finalDate = LocalDateTime.parse(json.getString("Final_date"));
      res = new Task(name, tags, initialDate, finalDate);
    }

    // Generates the intervals that are in a task and assign their parents to him
    for (int i = 0; i < intervals.length(); i++) {
      Interval interval = makeInterval(intervals.getJSONObject(i));
      interval.setParent(res);
      intervalList.add(interval);
    }
    res.setIntervalList(intervalList);
    return res;
  }

  private Project makeProject(JSONObject json) {
    // Is a method that generates Projects objects by parsing
    // the json, is recursive cuz a project can be inside
    // another project

    List<Activity> childArray = new ArrayList<>();
    String name = json.getString("name");

    JSONArray tagList = json.getJSONArray("tags");
    List<String> tags = new ArrayList<>();
    for (int i = 0; i < tagList.length(); i++) {
      tags.add(tagList.getString(i));
    }

    String stringDate = json.getString("Initial_date");
    Project res;
    if (stringDate.equals("null")) {
      res = new Project(name, tags, null, null);
    } else {
      LocalDateTime initialDate = LocalDateTime.parse(stringDate);
      LocalDateTime finalDate = LocalDateTime.parse(json.getString("Final_date"));
      res = new Project(name, tags, initialDate, finalDate);
    }

    // Iterates children and if is project call this method again if
    // it is a task call makeTask method. In both cases assign the parent
    // to the current project that has been made.
    JSONArray children = json.getJSONArray("children");
    for (int i = 0; i < children.length(); i++) {
      JSONObject child = children.getJSONObject(i);
      if (child.getString("class").equals("Project")) {
        Project project = makeProject(child);
        project.setParent(res);
        childArray.add(project);
      } else {
        Task task = makeTask(child);
        task.setParent(res);
        childArray.add(task);
      }
    }
    res.setChildren(childArray);
    return res;
  }

  public Project upload(String nameFile) {
    // This function reads the variables from the JSON file and
    // return a tree with the projects and tasks.
    String s = "";
    try {
      s = Files.readString(Paths.get(nameFile));
      logger.info("Successfully upload the json");
    } catch (Exception e) {
      logger.error("Error while uploading json");
      System.exit(-1);
    }

    return makeProject(new JSONObject(new JSONTokener(s)));
  }
}
