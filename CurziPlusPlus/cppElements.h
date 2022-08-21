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
};

CPPelementClass(Class)
	virtual void toString(int ntab) override;
};

CPPelementClass(Function)
	virtual void toString(int ntab) override;
};

CPPelementClass(Typename)
	virtual void toString(int ntab) override;
};

CPPelementClass(TemplateTypename)
	virtual void toString(int ntab) override;
};

CPPelementClass(NSTypename)
	virtual void toString(int ntab) override;
};

CPPelementClass(Method)
	virtual void toString(int ntab) override;
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
};

CPPelementClass(ClassInheritance)
	virtual void toString(int ntab) override;
};

CPPelementClass(MultipleInheritance)
	virtual void toString(int ntab) override;
};

CPPelementClass(TypenameList)
	virtual void toString(int ntab) override;
};

CPPelementClass(Indent)
	virtual void toString(int ntab) override;
};
