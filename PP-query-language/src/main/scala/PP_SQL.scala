import scala.language.implicitConversions

/* Clasele care implementeaza acest trait reprezinta niste wrappere peste
* functiile implementate din Database */
trait PP_SQL_DB{
  def eval: Option[Database]
}

case class CreateTable(database: Database, tableName: String) extends PP_SQL_DB{
  def eval: Option[Database] = Some(database.create(tableName))
}

case class DropTable(database: Database, tableName: String) extends PP_SQL_DB{
  def eval: Option[Database] = Some(database.drop(tableName))
}

implicit def PP_SQL_DB_Create_Drop(t: (Option[Database], String, String)): Option[PP_SQL_DB] = {
  val (optionDatabase, op, tableName) = t
  optionDatabase match {
    case Some(database) =>
      if (op.equals("CREATE")) Some(CreateTable(database, tableName))
      else if (op.equals("DROP")) Some(DropTable(database, tableName))
      else None
    case None => None
  }
}

case class SelectTables(database: Database, tableNames: List[String]) extends PP_SQL_DB{
  def eval: Option[Database] = database.selectTables(tableNames)
}

implicit def PP_SQL_DB_Select(t: (Option[Database], String, List[String])): Option[PP_SQL_DB] = {
  val (optionDatabase, op, tableNames) = t
  optionDatabase match {
    case Some(database) =>
      if (op.equals("SELECT")) Some(SelectTables(database, tableNames))
      else None
    case None => None
  }
}

case class JoinTables(database: Database, table1: String, column1: String, table2: String, column2: String) extends PP_SQL_DB{
  def eval: Option[Database] = database.join(table1, column1, table2, column2) match {
    case Some(table) => Some(Database(List(table)))
    case None => None
  }
}

implicit def PP_SQL_DB_Join(t: (Option[Database], String, String, String, String, String)): Option[PP_SQL_DB] = {
  val (optionDatabase, op, table1, column1, table2, column2) = t
  optionDatabase match {
    case Some(database) =>
      if (op.equals("JOIN")) Some(JoinTables(database, table1, column1, table2, column2))
      else None
    case None => None
  }
}

/* Clasele care implementeaza acest trait reprezinta niste wrappere peste
* functiile implementate din Table */
trait PP_SQL_Table{
  def eval: Option[Table]
}

case class InsertRow(table:Table, values: Tabular) extends PP_SQL_Table{
  def eval: Option[Table] = {
    val newTable = values.foldLeft(table)((acc, row) => acc.insert(row))
    Some(newTable)
  }
}

implicit def PP_SQL_Table_Insert(t: (Option[Table], String, Tabular)): Option[PP_SQL_Table] = {
  val (optionTable, op, values) = t
  optionTable match {
    case Some(table) =>
      if (op.equals("INSERT")) Some(InsertRow(table, values))
      else None
    case None => None
  }
}

case class UpdateRow(table: Table, condition: FilterCond, updates: Map[String, String]) extends PP_SQL_Table{
  def eval: Option[Table] = Some(table.update(condition, updates))
}

implicit def PP_SQL_Table_Update(t: (Option[Table], String, FilterCond, Map[String, String])): Option[PP_SQL_Table] = {
  val (optionTable, op, cond, updates) = t
  optionTable match {
    case Some(table) =>
      if (op.equals("UPDATE")) Some(UpdateRow(table, cond, updates))
      else None
    case None => None
  }
}

case class SortTable(table: Table, column: String) extends PP_SQL_Table{
  def eval: Option[Table] = Some(table.sort(column))
}

implicit def PP_SQL_Table_Sort(t: (Option[Table], String, String)): Option[PP_SQL_Table] = {
  val (optionTable, op, column) = t
  optionTable match {
    case Some(table) =>
      if (op.equals("SORT")) Some(SortTable(table, column))
      else None
    case None => None
  }
}

case class DeleteRow(table: Table, row: Row) extends PP_SQL_Table{
  def eval: Option[Table] = Some(table.delete(row))
}

implicit def PP_SQL_Table_Delete(t: (Option[Table], String, Row)): Option[PP_SQL_Table] = {
  val (optionTable, op, row) = t
  optionTable match {
    case Some(table) =>
      if (op.equals("DELETE")) Some(DeleteRow(table, row))
      else None
    case None => None
  }
}

case class FilterRows(table: Table, condition: FilterCond) extends PP_SQL_Table{
  def eval: Option[Table] = Some(table.filter(condition))
}

implicit def PP_SQL_Table_Filter(t: (Option[Table], String, FilterCond)): Option[PP_SQL_Table] = {
  val (optionTable, op, condition) = t
  optionTable match {
    case Some(table) =>
      if (op.equals("FILTER")) Some(FilterRows(table, condition))
      else None
    case None => None
  }
}

case class SelectColumns(table: Table, columns: List[String]) extends PP_SQL_Table{
  def eval: Option[Table] = Some(table.select(columns))
}

implicit def PP_SQL_Table_Select(t: (Option[Table], String, List[String])): Option[PP_SQL_Table] = {
  val (optionTable, op, columns) = t
  optionTable match {
    case Some(table) =>
      if (op.equals("EXTRACT")) Some(SelectColumns(table, columns))
      else None
    case None => None
  }
}

/* Functiile primesc o operatie sql pe un table sau databse returneaza rezultatul */
def queryT(p: Option[PP_SQL_Table]): Option[Table] = {
  p match {
    case Some(pp_table) => pp_table.eval
    case None => None
  }
}
def queryDB(p: Option[PP_SQL_DB]): Option[Database] = {
  p match {
    case Some(pp_db) => pp_db.eval
    case None => None
  }
}