import scala.language.implicitConversions

trait FilterCond {def eval(r: Row): Option[Boolean]}

case class Field(colName: String, predicate: String => Boolean) extends FilterCond {
  override def eval(r: Row): Option[Boolean] = {
    r.get(colName) match {
      // se evalueaza functia predicate pentru valoarea cheii
      case Some(str) => Some(predicate(str))
      case None => None // nu exista elementul asociat cheii pe acest rand
    }
  }
}

case class Compound(op: (Boolean, Boolean) => Boolean, conditions: List[FilterCond]) extends FilterCond {
  override def eval(r: Row): Option[Boolean] = {
    // se evalueaza prima conditie din lista
    val acc = conditions.head.eval(r)
    acc match {
      /* se evalueaza restul conditiilor, iar rezultatele se compun intr-un foldLeft */
      case Some(_) => conditions.tail.foldLeft(acc) { (accumulator, cond) =>
        accumulator match {
          case Some(value) =>
            cond.eval(r) match {
              case Some(condValue) => Some(op(value, condValue))
              case None => None
            }
          case None => None
        }
      }
      case None => None
    }
  }
}

case class Not(f: FilterCond) extends FilterCond {
  override def eval(r: Row): Option[Boolean] = {
    /* daca functia eval intoarce un rezultat, se returneaza rezultatul negat */
    val result = f.eval(r)
    result match {
      case Some(booleanVal) => Some(!booleanVal)
      case None => None
    }
  }
}

/* folosind case class-ul Compound, se definesc aceste filtre drept compuneri de alte doua filtre */
def And(f1: FilterCond, f2: FilterCond): FilterCond = Compound(_&&_, f1::f2::Nil)
def Or(f1: FilterCond, f2: FilterCond): FilterCond = Compound(_||_, f1::f2::Nil)
def Equal(f1: FilterCond, f2: FilterCond): FilterCond = Compound((x, y) => x == y, f1::f2::Nil)


case class Any(fs: List[FilterCond]) extends FilterCond {
  override def eval(r: Row): Option[Boolean] = {
    // se compun toate filtrele cu "sau" si dupa se evalueaza
    Compound(_||_, fs).eval(r)
  }
}

case class All(fs: List[FilterCond]) extends FilterCond {
  override def eval(r: Row): Option[Boolean] = {
    // se compun toate filtrele cu "si" si dupa se evalueaza
    Compound(_&&_, fs).eval(r)
  }
}

implicit def tuple2Field(t: (String, String => Boolean)): Field = Field(t._1, t._2)

extension (f: FilterCond) {
  def ===(other: FilterCond) = Equal(f,other)
  def &&(other: FilterCond) = And(f,other)
  def ||(other: FilterCond) = Or(f,other)
  def !! = Not(f)
}