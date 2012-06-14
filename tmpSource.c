    qreal pastezValue = maxValue + 1;
    for (int m = 0; m < itList.size(); m++)
    {
        itList.at(m)->setZValue(pastezValue++);
        scene->addItem(itList.at(m));
    }
    if (select)
        selectItems(items);