// { dg-do compile }
// { dg-options "-fgnu-tm" }

void foo(){
    __transaction {
        throw 5;
    }
}
