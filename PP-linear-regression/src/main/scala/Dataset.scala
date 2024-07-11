import scala.io.Source

class Dataset(m: List[List[String]]) {
  val data: List[List[String]] = m

  /* Creates a string representation of an Dataset object */
  /* This function is inspired from the toString function for Tabular from Curs06 */
  override def toString: String = m.map(_.foldRight(""){
    case (x, "") => s"$x" // mapping for the last element of a list
    case (x, y) => s"$x,$y"
    }).foldRight("")(_ + "\n" + _)

  /* Creates a new Dataset that contains only the column with the given String as header */
  def selectColumn(col: String): Dataset = {
    /* Calculates the index of the given string in header
     * Using foldLeft on data, a new Dataset is constructed that contains only the element
     * with the same index as the given string
     */
    val index = getHeader.indexOf(col)
    new Dataset(data.foldLeft(Nil: List[List[String]])((acc, row) => acc :+ List(row(index))))

    /* Another implementation using foldRight
     * new Dataset(data.foldRight(Nil: List[List[String]])((acc, row) => List(row(index)) :: acc))
     */
  }

  /* Creates a new Dataset that contains the columns with the given list of strings as header */
  def selectColumns(cols: List[String]): Dataset = {
    /* Creates a lists with all the results obtained from applying selectColumn on all the
     * strings contained in the list. Afterwards, uses foldLeft to create a new list from the
     * one, where each component list is added to the accumulator with zip, and the pairs resulted
     * are mapped to list concatenation.
     */
    val listCols = cols.map(col => selectColumn(col).data)
    val data = listCols.tail.foldLeft(listCols.head)((acc, list) => acc.zip(list).map((e1, e2) => e1 ++ e2))
    new Dataset(data)
  }

  /* Splits the current dataset into two, based on the given parameter */
  def split(percentage: Double): (Dataset, Dataset) = {
    val num_vals = 1/percentage - 1;
    /* Sorting the list after the first column */
    val sortedDataset = getRows.sortBy(_.head)

    /* The sorted data is split into two using fold left. The accumulator is a tuple that contains
     * the two resulting list and a value that is used for testing in which list should the next element be added. */
    val (trainData, testData, _) = sortedDataset.foldLeft((Nil: List[List[String]], Nil: List[List[String]], 0))((acc, row) => {
      val (train, test, num) = acc;
      if (num < num_vals) (train :+ row, test, num + 1)
      else (train, test :+ row, 0)
    })

    /* Creating the resulting datasets */
    val dataset1 = new Dataset(getHeader :: trainData)
    val dataset2 = new Dataset(getHeader :: testData)
    (dataset1, dataset2)
  }

  /* Functions for getting information related to the dataset */
  def size: Int = m.size - 1
  def getRows: List[List[String]] = m.tail
  def getHeader: List[String] = m.head
}

object Dataset {
  /* Creates a new Dataset from a csv file */
  def apply(csv_filename: String): Dataset = {
    /* Creating the buffered source */
    val buffer = Source.fromFile(csv_filename)

    /* Creating a dataset from the lines of the file after splitting them */
    val result = new Dataset(buffer.getLines.toList.map(_.split(",").toList))

    /* Closing the source and returning the result */
    buffer.close()
    result
  }

  /* Creates a new Dataset from a list of lists of strings */
  def apply(ds: List[List[String]]): Dataset = {
    new Dataset(ds)
  }
}
