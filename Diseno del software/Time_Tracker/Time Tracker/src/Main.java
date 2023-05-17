import core.ClockTimer;
import core.Json;
import core.Printer;
import core.Project;
import core.Task;
import java.lang.Thread;
import java.util.ArrayList;
import java.util.List;
import utils.SearchByTag;

public class Main {
  private static void sleep(int value) {
    try {
      Thread.sleep(value);
    } catch (InterruptedException e) {
      System.exit(0);
    }
  }

  private static void testA() {
    Project sd = new Project("software design");
    sd.addTag("java");
    sd.addTag("flutter");

    Project st = new Project("software testing");
    st.addTag("c++");
    st.addTag("Java");
    st.addTag("python");

    Project db = new Project("databases");
    db.addTag("SQL");
    db.addTag("python");
    db.addTag("C++");

    Project root = new Project("root");
    Task trans = new Task("transportation");

    root.addChild(sd);
    root.addChild(st);
    root.addChild(db);
    root.addChild(trans);

    Project prob = new Project("problems");
    Project ptt = new Project("project time tracker");

    sd.addChild(prob);
    sd.addChild(ptt);

    Task fl = new Task("first list");
    fl.addTag("java");

    Task sl = new Task("second list");
    sl.addTag("Dart");

    prob.addChild(fl);
    prob.addChild(sl);

    Task rh = new Task("read handout");
    Task fm = new Task("first milestone");
    fm.addTag("Java");
    fm.addTag("IntelliJ");

    ptt.addChild(rh);
    ptt.addChild(fm);

    Printer printer = new Printer();
    root.accept(printer);

    List<String> tags = new ArrayList<>();
    tags.add("java");
    tags.add("JAVA");
    tags.add("intellij");
    tags.add("c++");
    tags.add("python");

    SearchByTag searchByTag = new SearchByTag(tags);
    root.accept(searchByTag);

  }

  private static void testB() {

    Project sd = new Project("software design");
    Project st = new Project("software testing");
    Project root = new Project("root");
    Project db = new Project("databases");
    Task transportation = new Task("transportation");
    root.addChild(db);
    root.addChild(transportation);
    root.addChild(st);
    root.addChild(sd);
    st.addTag("c++");
    db.addTag("C++");
    st.addTag("Java");
    db.addTag("SQL");
    st.addTag("python");
    sd.addTag("java");
    db.addTag("python");
    sd.addTag("flutter");



    Project prob = new Project("problems");
    sd.addChild(prob);
    Project ptt = new Project("project time tracker");
    sd.addChild(ptt);

    Task secondList = new Task("second list");
    Task firstList = new Task("first list");
    prob.addChild(secondList);
    prob.addChild(firstList);
    firstList.addTag("java");
    secondList.addTag("Dart");

    Task rh = new Task("read handout");
    Task fm = new Task("first milestone");
    fm.addTag("Java");
    fm.addTag("IntelliJ");

    ptt.addChild(rh);
    ptt.addChild(fm);

    ClockTimer.getInstance();
    sleep(1500);

    transportation.start();
    sleep(6000);

    transportation.stop();
    sleep(2000);

    firstList.start();
    sleep(6000);

    secondList.start();
    sleep(4000);

    firstList.stop();
    sleep(2000);

    secondList.stop();
    sleep(2000);

    transportation.start();
    sleep(4000);

    transportation.stop();

    Json json = new Json();
    json.save(root, "./save.json");
  }

  private static void testC() {
    Json json = new Json();
    Project root = json.upload("save.json");

    Printer printer = new Printer();
    root.accept(printer);

    List<String> tags = new ArrayList<>();
    tags.add("java");
    tags.add("c++");
    tags.add("intellij");
    tags.add("JAVA");
    tags.add("python");

    SearchByTag searchByTag = new SearchByTag(tags);
    root.accept(searchByTag);
  }

  public static void main(String[] args) {
    testC();
    System.exit(0);
  }
}