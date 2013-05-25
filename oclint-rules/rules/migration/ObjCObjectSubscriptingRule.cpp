#include "oclint/AbstractASTVisitorRule.h"
#include "oclint/RuleSet.h"

class ObjCObjectSubscriptingRule : public AbstractASTVisitorRule<ObjCObjectSubscriptingRule>
{
private:
    static RuleSet rules;

    bool containsInVector(string selectorName, vector<string> &selectors)
    {
        return find(selectors.begin(), selectors.end(), selectorName) != selectors.end();
    }

    bool isArrayGetSelector(Expr *expr, string &selectorString)
    {
        vector<string> arrayGetSelectors;
        arrayGetSelectors.push_back("objectAtIndex:");
        arrayGetSelectors.push_back("objectAtIndexedSubscript:");

        return expr->getType().getAsString() == "NSArray *" &&
            containsInVector(selectorString, arrayGetSelectors);
    }

    bool isArraySetSelector(Expr *expr, string &selectorString)
    {
        vector<string> arraySetSelectors;
        arraySetSelectors.push_back("replaceObjectAtIndex:withObject:");
        arraySetSelectors.push_back("setObject:atIndexedSubscript:");

        return expr->getType().getAsString() == "NSMutableArray *" &&
            containsInVector(selectorString, arraySetSelectors);
    }

    bool isDictionaryGetSelector(Expr *expr, string &selectorString)
    {
        vector<string> dictionaryGetSelectors;
        dictionaryGetSelectors.push_back("objectForKey:");
        dictionaryGetSelectors.push_back("objectForKeyedSubscript:");

        return expr->getType().getAsString() == "NSDictionary *" &&
            containsInVector(selectorString, dictionaryGetSelectors);
    }

    bool isDictionarySetSelector(Expr *expr, string &selectorString)
    {
        vector<string> dictionarySetSelectors;
        dictionarySetSelectors.push_back("setObject:forKey:");
        dictionarySetSelectors.push_back("setObject:forKeyedSubscript:");

        return expr->getType().getAsString() == "NSMutableDictionary *" &&
            containsInVector(selectorString, dictionarySetSelectors);
    }

public:
    virtual const string name() const
    {
        return "replace with object subscripting";
    }

    virtual int priority() const
    {
        return 3;
    }

    bool VisitObjCMessageExpr(ObjCMessageExpr *objCMsgExpr)
    {
        Expr *receiverExpr = objCMsgExpr->getInstanceReceiver();
        string selectorString = objCMsgExpr->getSelector().getAsString();

        if (receiverExpr && isa<ImplicitCastExpr>(receiverExpr) &&
            (isArrayGetSelector(receiverExpr, selectorString) ||
            isArraySetSelector(receiverExpr, selectorString) ||
            isDictionaryGetSelector(receiverExpr, selectorString) ||
            isDictionarySetSelector(receiverExpr, selectorString)))
        {
            addViolation(objCMsgExpr, this);
        }

        return true;
    }

};

RuleSet ObjCObjectSubscriptingRule::rules(new ObjCObjectSubscriptingRule());
