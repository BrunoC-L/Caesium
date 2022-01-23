#pragma once
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include "factory.h"
#include "cppElement.h"

CPPelementClass(File)
	virtual void toString(int ntab) override;
	virtual void visit(Class* e) override;
	virtual void visit(Function* e) override;
};

CPPelementClass(Class)
	virtual void toString(int ntab) override;
	virtual void visit(Typename* e) override;
	virtual void visit(ClassMember* e) override;
	virtual void visit(Constructor* e) override;
};

CPPelementClass(Function)
	virtual void toString(int ntab) override;
};

CPPelementClass(Typename)
	virtual void toString(int ntab) override;
	virtual void visit(TemplateTypename* e) override;
	virtual void visit(NSTypename* e) override;
};

CPPelementClass(TemplateTypename)
	virtual void toString(int ntab) override;
	virtual void visit(Typename* e) override;
};

CPPelementClass(NSTypename)
	virtual void toString(int ntab) override;
	virtual void visit(Typename* e) override;
};

CPPelementClass(Method)
	virtual void toString(int ntab) override;
	virtual void visit(Typename* e) override;
};

CPPelementClass(MemberVariable)
	virtual void toString(int ntab) override;
};

CPPelementClass(ClassElement)
	virtual void toString(int ntab) override;
};

CPPelementClass(Constructor)
	virtual void toString(int ntab) override;
};

CPPelementClass(ClassMember)
	virtual void toString(int ntab) override;
	virtual void visit(Method* e) override;
	virtual void visit(MemberVariable* e) override;
};
